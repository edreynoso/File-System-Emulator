#ifndef HELPER_H
#define HELPER_H
#define MAX_INODE 1024
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    file,
    directory
} file_type;

struct file_info
{
    char fname[33];
    file_type file_type;
    int iNode;
    struct file_info *children[MAX_INODE];
    int child_count;
};

typedef struct file_info file_info;

extern file_info *file_system[MAX_INODE];
extern int current_directory;
extern bool freed[MAX_INODE];

char *uint32_to_str(uint32_t i);
int get_directory_inode(const char *name);
int check_available(const char *name);
int get_next_inode();
void exit_free(file_info *f);
int exit_program(const char *root);
int update_directory(const char *filename, const int inode);
int parse_file(const char *target, file_info *f);

#endif