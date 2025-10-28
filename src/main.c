#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <sys/stat.h>

void init_repo();

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: dotman <command> [args]\n");

    return 1;
  }

  if (strcmp(argv[1], "init") == 0) {
    printf("Initializing dotman repository...\n");
    init_repo();
    // TODO: call init function
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      printf("Usage: dotman add <file>\n");
      return 1;
    }
    printf("Adding file: %s\n", argv[2]);
    // TODO: call add function
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
