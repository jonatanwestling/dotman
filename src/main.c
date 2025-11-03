/**
 * @file main.c
 * @author Jonatan Westling
 * @brief Main file for the dotman project
 * @version 0.1
 * @date 2025-11-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/_types/_ssize_t.h>
#include <unistd.h>

#include <stdlib.h>
#include <sys/stat.h>

// add files
#include <errno.h>
#include <libgen.h> // for basename()
#include <unistd.h> // for symlink(), access()

void init_repo();
void add_file(const char *filepath);
int check_file_status(const char *filepath);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: dotman <command> [args]\n");
    return 1;
  }

  if (strcmp(argv[1], "init") == 0) {
    printf("Initializing dotman repository...\n");
    init_repo();
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      printf("Usage: dotman add <file>\n");
      return 1;
    }
    printf("Adding file: %s\n", argv[2]);
    add_file(argv[2]);
  } else if (strcmp(argv[1], "list") == 0) {
    printf("Listing tracked files...\n");
    // TODO: implement listing
  } else {
    printf("Unknown command: %s\n", argv[1]);
  }

  return 0;
}

void init_repo() {
  const char *home = getenv("HOME");
  if (!home) {
    printf("Could not get HOME directory.\n");
  } else {
    printf("HOME directory: %s\n", home);
  }
  char repo_path[512];
  snprintf(repo_path, sizeof(repo_path), "%s/.dotman", home);
  // create folders with basic permissions
  if (mkdir(repo_path, 0755) == 0) {
    printf("Dotman repository created at %s\n", repo_path);
    // Create subfolders
    char files_path[512];
    char backup_path[512];
    snprintf(files_path, sizeof(files_path), "%s/files", repo_path);
    snprintf(backup_path, sizeof(backup_path), "%s/backup", repo_path);
    mkdir(files_path, 0755);
    mkdir(backup_path, 0755);
  } else {
    perror("Failed to create repo");
  }
}

void add_file(const char *filepath) {
  printf("Filepath: %s\n", filepath);
  // get the absolute path of the original file
  char original_path[512];
  if (!realpath(filepath, original_path)) {
    perror("Failed to resolve absolute path");
    return;
  }
  printf("Absolute path: %s\n", original_path);

  // check if the file is a symlink
  int status = check_file_status(filepath);
  printf("Status: %d\n", status);
  if (status == 1) {
    // already tracked by dotman
    return;
  } else if (status == 3) {
    // user chose to skip external symlink
    return;
  } else if (status == 2) {
    // User chose to import external symlink
    // In this case, you may want to replace filepath with the target
    // e.g., filepath = target from check_file_status
  }

  // get the repo_path
  const char *home = getenv("HOME");
  if (!home) {
    printf("Could not get HOME directory.\n");
    return;
  }
  char repo_path[512];
  snprintf(repo_path, sizeof(repo_path), "%s/.dotman/files", home);

  // Get just the filename from the path
  char file_copy[256];
  strncpy(file_copy, filepath, sizeof(file_copy));
  char *filename = basename(file_copy);

  // Construct destionation paths
  char dest_path[512];
  snprintf(dest_path, sizeof(dest_path), "%s/%s", repo_path, filename);

  char backup_path[512];
  snprintf(backup_path, sizeof(backup_path), "%s/.dotman/backup/%s", home,
           filename);

  // Backup org file
  FILE *src = fopen(filepath, "r");
  FILE *backup = fopen(backup_path, "w");
  if (!src || !backup) {
    perror("Error opening files");
    return;
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

  // Move the org file to dotman
  rename(filepath, dest_path);

  if (access(filepath, F_OK) == 0) {
    // Remove existing file/symlink
    if (unlink(filepath) != 0) {
      perror("Failed to remove existing file");
      return;
    }
  }

  if (symlink(dest_path, filepath) == 0) {
    printf("Tracked and linked %s → %s\n", filepath, dest_path);
  } else {
    perror("Failed to create symlink");
  }
  // Log to dotman.db
  char db_path[512];
  snprintf(db_path, sizeof(db_path), "%s/.dotman/dotman.db", home);
  FILE *db = fopen(db_path, "a");
  if (db) {
    fprintf(db, "%s|%s\n", original_path, dest_path);
    fclose(db);
  } else {
    perror("Failed to write to dotman.db");
  }
}

/**
 * @brief Check the status of the given file path.
 * 
 * @param filepath 
 * @return int 0: normal file
 *             1: already tracked by dotman
 *             2: external symlink, user wants to import
 *             3: external symlink, user chooses to skip
 */
int check_file_status(const char *filepath) {
  struct stat path_stat;
  // Check if the given path exists and is a symbolic link
  if (lstat(filepath, &path_stat) == 0 &&
      (path_stat.st_mode & S_IFMT) == S_IFLNK) {
    char target[512];
    ssize_t len = readlink(filepath, target, sizeof(target) - 1);

    if (len == -1) {
      perror("readlink failed");
      return -1;
    }

    // null terminate target
    target[len] = '\0';

    printf("%s is a symlink → %s\n", filepath, target);

    // case 1: already tracked by dotman
    const char *home = getenv("HOME");
    if (home && strstr(target, "/.dotman/files") != NULL) {
      printf("Already tracked by dotman. Skipping.\n");
      return 1;
    }

    // case 2: external symlink
    printf("Do you want to import the target file (%s) into Dotman? [y/N]: ",
           target);
    char response = getchar();
    if (response == 'y' || response == 'Y') {
      printf("Importing target: %s\n", target);
      // user wants to import
      return 2;
    } else {
      printf("Skipping %s.\n", filepath);
      // user chooses to skip
      return 3;
    }
  }
  return 0;
}
