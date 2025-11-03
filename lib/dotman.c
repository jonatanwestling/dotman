/**
 * @file dotman.c
 * @author Jonatan Westling
 * @brief 
 * @version 0.1
 * @date 2025-11-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "dotman.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>

/**
 * @brief check the status of the given file path.
 * 
 * @param filepath 
 * @return int : 0 - normal file
 *               1 - already tracked by dotman
 *               2 - external symlink, user wants to import
 *               3 - external symlink, user chooses to skip
 *              -1 - error
 */
static int check_file_status(const char *filepath) {
  struct stat path_stat;
  if (lstat(filepath, &path_stat) == 0 && (path_stat.st_mode & S_IFMT) == S_IFLNK) {
    char target[PATH_MAX];
    ssize_t len = readlink(filepath, target, sizeof(target) - 1);
    if (len == -1) {
      perror("readlink failed");
      return -1;
    }
    target[len] = '\0';
    printf("%s is a symlink → %s\n", filepath, target);

    const char *home = getenv("HOME");
    if (home && strstr(target, "/.dotman/files") != NULL) {
      printf("Already tracked by dotman. Skipping.\n");
      return 1;
    }

    printf("Do you want to import the target file (%s) into Dotman? [y/N]: ", target);
    int c = getchar();
    /* consume leftover newline if any */
    while (c != '\n' && c != EOF) break;
    if (c == 'y' || c == 'Y') {
      printf("Importing target: %s\n", target);
      return 2;
    } else {
      printf("Skipping %s.\n", filepath);
      return 3;
    }
  }
  return 0;
}

/**
 * @brief Initialize the dotman repository.
 * 
 * @return int 0 on success, non-zero on error
 */
int dotman_init(void) {
  const char *home = getenv("HOME");
  if (!home) {
    fprintf(stderr, "Could not get HOME directory.\n");
    return -1;
  }
  char repo_path[PATH_MAX];
  snprintf(repo_path, sizeof(repo_path), "%s/.dotman", home);

  if (mkdir(repo_path, 0755) == 0) {
    printf("Dotman repository created at %s\n", repo_path);
    char files_path[PATH_MAX];
    char backup_path[PATH_MAX];
    snprintf(files_path, sizeof(files_path), "%s/files", repo_path);
    snprintf(backup_path, sizeof(backup_path), "%s/backup", repo_path);
    mkdir(files_path, 0755);
    mkdir(backup_path, 0755);
  } else if (errno == EEXIST) {
    /* already exists, that's fine */
    printf("Dotman repository already exists at %s\n", repo_path);
  } else {
    perror("Failed to create repo");
    return -1;
  }
  return 0;
}

/**
 * @brief Add a file to the dotman repository and replace it with a symlink.
 * 
 * @param filepath
 * @return int 0 on success, non-zero on error
 */
int dotman_add(const char *filepath) {
  if (!filepath) return -1;
  printf("Filepath: %s\n", filepath);

  char original_path[PATH_MAX];
  if (!realpath(filepath, original_path)) {
    perror("Failed to resolve absolute path");
    return -1;
  }
  printf("Absolute path: %s\n", original_path);

  int status = check_file_status(filepath);
  printf("Status: %d\n", status);
  if (status == 1 || status == 3) {
    return 0;
  }

  const char *home = getenv("HOME");
  if (!home) {
    fprintf(stderr, "Could not get HOME directory.\n");
    return -1;
  }
  char repo_path[PATH_MAX];
  snprintf(repo_path, sizeof(repo_path), "%s/.dotman/files", home);

  char file_copy[PATH_MAX];
  strncpy(file_copy, filepath, sizeof(file_copy));
  file_copy[sizeof(file_copy)-1] = '\0';
  char *filename = basename(file_copy);

  char dest_path[PATH_MAX];
  snprintf(dest_path, sizeof(dest_path), "%s/%s", repo_path, filename);

  char backup_path[PATH_MAX];
  snprintf(backup_path, sizeof(backup_path), "%s/.dotman/backup/%s", home, filename);

  FILE *src = fopen(filepath, "r");
  FILE *backup = fopen(backup_path, "w");
  if (!src || !backup) {
    perror("Error opening files");
    if (src) fclose(src);
    if (backup) fclose(backup);
    return -1;
  }

  char buffer[512];
  size_t bytes;
  size_t total = 0;
  while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
    fwrite(buffer, 1, bytes, backup);
    total += bytes;
    printf("\rBacking up %s: %zu bytes copied", filepath, total);
    fflush(stdout);
  }
  printf("\nBackup completed!\n");

  fclose(src);
  fclose(backup);

  if (rename(filepath, dest_path) != 0) {
    perror("Failed to move file to repo");
    return -1;
  }

  if (access(filepath, F_OK) == 0) {
    if (unlink(filepath) != 0) {
      perror("Failed to remove existing file");
      return -1;
    }
  }

  if (symlink(dest_path, filepath) == 0) {
    printf("Tracked and linked %s → %s\n", filepath, dest_path);
  } else {
    perror("Failed to create symlink");
  }

  char db_path[PATH_MAX];
  snprintf(db_path, sizeof(db_path), "%s/.dotman/dotman.db", home);
  FILE *db = fopen(db_path, "a");
  if (db) {
    fprintf(db, "%s|%s\n", original_path, dest_path);
    fclose(db);
  } else {
    perror("Failed to write to dotman.db");
  }

  return 0;
}
