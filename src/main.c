#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/stat.h>

// add files
#include <libgen.h> // for basename()
#include <unistd.h> // for symlink(), access()

void init_repo();
void add_file(const char *filepath);

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
  if (symlink(filepath, dest_path) == 0) {
    printf("Tracked and linked %s → %s\n", filepath, filename);
  } else {
    perror("Failed to create symlink");
  }
}
