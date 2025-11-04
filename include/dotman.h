/*
 * Public API for dotman library
 */
#ifndef DOTMAN_H
#define DOTMAN_H

#include <stddef.h>

/* Initialize the dotman repository under $HOME/.dotman
 * Returns 0 on success, non-zero on error.
 */
int dotman_init(void);

/* Add a file to the dotman repository and replace it with a symlink.
 * Returns 0 on success, non-zero on error.
 */
int dotman_add(const char *filepath);

int check_file_status(const char *filepath);

int dotman_list_tracked_files(void);

#endif /* DOTMAN_H */

