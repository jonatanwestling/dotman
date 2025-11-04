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
#include <errno.h>
#include <libgen.h>
#include <unistd.h>

#include "dotman.h"

int main(int argc, char *argv[]) {

  if (argc < 2) {
    printf("Usage: dotman <command> [args]\n");
    return 1;
  }

  if (strcmp(argv[1], "init") == 0) {
    printf("Initializing dotman repository...\n");
     dotman_init();
  } else if (strcmp(argv[1], "add") == 0) {
    if (argc < 3) {
      printf("Usage: dotman add <file>\n");
      return 1;
    }
    printf("Adding file: %s\n", argv[2]);
    dotman_add(argv[2]);
  } else if (strcmp(argv[1], "list") == 0) {
    printf("Listing tracked files...\n");
    dotman_list_tracked_files();
  } else {
    printf("Unknown command: %s\n", argv[1]);
  }

  return 0;
}
