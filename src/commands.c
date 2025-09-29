#include "../lib/commands.h"
#include "../lib/helper.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>

int cd(const int inode)
{
    current_directory = inode;
    return EXIT_SUCCESS;
}

int ls(const int inode)
{
    if (file_system[inode] != NULL)
    {
        file_info *dir = file_system[inode];
        for (int i = 0; i < dir->child_count; i++)
        {
            file_info *child = dir->children[i];
            int in = child->iNode;
            printf("|----%d %s\n", in, child->fname);
        }
    }
    return EXIT_SUCCESS;
}

int mk_dir(const char *name)
{
    if (check_available(name) != -1)
    {
        return EXIT_FAILURE;
    }
    int inode = get_next_inode();
    file_info *dir = malloc(sizeof(file_info));
    dir->iNode = inode;
    dir->file_type = directory;
    dir->child_count = 0;
    strcpy(dir->fname, name);
    file_system[inode] = dir;

    file_info *parent = file_system[current_directory];

    parent->children[parent->child_count] = dir;
    parent->child_count++;

    file_info *self_dot = malloc(sizeof(file_info));
    strcpy(self_dot->fname, ".\0");
    self_dot->file_type = directory;
    self_dot->iNode = dir->iNode;

    file_info *parent_dot = malloc(sizeof(file_info));
    strcpy(parent_dot->fname, "..\0");
    parent_dot->file_type = directory;
    parent_dot->iNode = parent->iNode;

    dir->children[dir->child_count] = self_dot;
    dir->child_count++;
    dir->children[dir->child_count] = parent_dot;
    dir->child_count++;

    char *dir_filename = uint32_to_str(inode);
    int fd = open(dir_filename, O_TRUNC | O_CREAT | O_RDWR, 0700);
    if (fd == -1)
    {
        perror("Failed to create directory file");
        free(dir_filename);
        return EXIT_FAILURE;
    }

    char entry[36];

    memcpy(entry, &dir->iNode, sizeof(uint32_t));
    memset(entry + 4, 0, 32);
    strncpy((char *)(entry + 4), ".", 32);
    write(fd, entry, 36);

    memcpy(entry, &parent->iNode, sizeof(uint32_t));
    memset(entry + 4, 0, 32);
    strncpy((char *)(entry + 4), "..", 32);
    write(fd, entry, 36);

    close(fd);
    free(dir_filename);

    update_directory(name, inode);

    return EXIT_SUCCESS;
}

int touch(const char *name)
{
    if (check_available(name) != -1)
    {
        return EXIT_FAILURE;
    }
    int inode = get_next_inode();

    file_info *f = malloc(sizeof(file_info));
    f->iNode = inode;
    f->file_type = file;
    strcpy(f->fname, name);

    file_system[inode] = f;

    file_info *current = file_system[current_directory];
    current->children[current->child_count] = f;
    current->child_count++;

    char *num_name = uint32_to_str(inode);
    int fd;
    fd = open(num_name, O_TRUNC | O_CREAT | O_RDWR, 0700);
    write(fd, f->fname, strlen(f->fname));
    close(fd);
    update_directory(f->fname, inode);

    free(num_name);
    return EXIT_SUCCESS;
}