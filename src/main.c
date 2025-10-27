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
}
