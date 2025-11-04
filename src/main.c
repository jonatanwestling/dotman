#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dotman.h"

typedef enum {
    CMD_UNKNOWN,
    CMD_INIT,
    CMD_ADD,
    CMD_LIST,
    CMD_UNLINK,
    CMD_HELP,
} Command;

Command parse_command(const char *arg) {
    if (strcmp(arg, "init") == 0) return CMD_INIT;
    if (strcmp(arg, "add") == 0) return CMD_ADD;
    if (strcmp(arg, "list") == 0) return CMD_LIST;
    if (strcmp(arg, "unlink") == 0) return CMD_UNLINK;
    if (strcmp(arg, "help") == 0) return CMD_HELP;
    return CMD_UNKNOWN;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: dotman <command> [args]\n");
        printf("Use 'dotman help' for a list of commands.\n");
        return 1;
    }
    // get the command
    Command cmd = parse_command(argv[1]);

    switch (cmd) {
        case CMD_INIT:
            printf("Initializing dotman repository...\n");
            dotman_init();
            break;

        case CMD_ADD:
            if (argc < 3) {
                printf("Usage: dotman add <file>\n");
                printf("Use 'dotman help' for a list of commands.\n");
                return 1;
            }
            printf("Adding file: %s\n", argv[2]);
            dotman_add(argv[2]);
            break;

        case CMD_LIST:
            printf("Listing tracked files...\n");
            dotman_list_tracked_files();
            break;

        case CMD_UNLINK:
            if (argc < 3) {
                printf("Usage: dotman unlink <file>\n");
                return 1;
            }
            printf("Unlinking file: %s\n", argv[2]);
            break;

        case CMD_HELP:
            printf("dotman - A simple dotfile manager\n");
            printf("Usage: dotman <command> [args]\n");
            printf("Commands:\n");
            printf("  init           Initialize the dotman repository\n");
            printf("  add <file>     Add a file to the dotman repository\n");
            printf("  list           List all tracked files\n");
            printf("  unlink <file>  Unlink a tracked file\n");
            printf("  help           Show this help message\n");
            break;

        case CMD_UNKNOWN:
        default:
            printf("Unknown command: %s\n", argv[1]);
            printf("Use 'dotman help' for a list of commands.\n");
            break;
    }

    return 0;
}