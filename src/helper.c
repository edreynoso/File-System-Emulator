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
#include <stdint.h>

file_info *file_system[MAX_INODE];

int current_directory = 0;

bool freed[MAX_INODE] = {false};

int get_directory_inode(const char *name)
{
    file_info *current = file_system[current_directory];

    for (int i = 0; i < current->child_count; i++)
    {
        file_info *child = current->children[i];

        if (file_system[child->iNode] != NULL && strcmp(child->fname, name) == 0 && file_system[child->iNode]->file_type == directory)
            return child->iNode;
    }

    return -1;
}

int check_available(const char *name)
{
    file_info *current = file_system[current_directory];

    for (int i = 0; i < current->child_count; i++)
    {
        file_info *child = current->children[i];

        if (strcmp(child->fname, name) == 0)
            return 0;
    }

    return -1;
}

int get_next_inode()
{
    int inode = 0;
    while (file_system[inode] != NULL)
    {
        inode++;
    }
    return inode;
}

char *uint32_to_str(uint32_t i)
{
    int length = snprintf(NULL, 0, "%lu", (unsigned long)i);

    char *str = malloc(sizeof(char) * length + 1);

    snprintf(str, length + 1, "%lu", (unsigned long)i);

    return str;
}

void exit_free(file_info *f)
{
    if (f == NULL || freed[f->iNode])
        return;

    freed[f->iNode] = true;

    if (f->file_type == directory)
    {
        for (int i = 0; i < f->child_count; i++)
        {
            file_info *child = f->children[i];

            if (file_system[child->iNode] != child)
            {
                exit_free(file_system[child->iNode]);
                free(child);
            }
            else
            {

                exit_free(child);
            }
        }
    }

    free(f);
}

int exit_program(const char *root)
{
    int fd = open("inodes_list", O_APPEND | O_TRUNC | O_RDWR);
    if (fd == -1)
    {
        printf("bad open in exit\n");
        return 1;
    }
    char file_data[5];
    for (int i = 0; i < MAX_INODE; i++)
    {
        if (file_system[i] != NULL)
        {
            file_info *f = file_system[i];
            uint32_t node = f->iNode;
            memcpy(file_data, &node, sizeof(uint32_t));
            if (file_system[i]->file_type == directory)
                file_data[4] = 'd';
            else
                file_data[4] = 'f';
            if (write(fd, file_data, 5) == -1)
            {
                perror("write() in exit");
                return EXIT_FAILURE;
            }
        }
    }

    for (int i = 0; i < MAX_INODE; i++)
    {
        if (file_system[i] != NULL && !freed[i])
        {
            exit_free(file_system[i]);
        }
    }
    close(fd);
    return EXIT_SUCCESS;
}

int update_directory(const char *filename, const int inode)
{
    char *directory_name = uint32_to_str(current_directory);
    int fd = open(directory_name, O_WRONLY | O_APPEND | O_CREAT, 0700);
    if (fd == -1)
    {
        printf("Could not update directory\n");
        return EXIT_FAILURE;
    }
    char new_f_info[36];
    memcpy(new_f_info, &inode, sizeof(uint32_t));
    memset(new_f_info + 4, 0, 32);
    strncpy((char *)(new_f_info + 4), filename, 32);
    write(fd, new_f_info, 36);
    close(fd);

    free(directory_name);
    return EXIT_SUCCESS;
}

int parse_file(const char *target, file_info *f)
{

    int fd = open(target, O_RDONLY);

    if (fd == -1)
    {
        perror("open() in parse_file\n");
        return EXIT_FAILURE;
    }

    int n;
    if (f->file_type == file)
    {
        char buffer[33];
        while ((n = read(fd, buffer, 32)) > 0)
        {
            memcpy(f->fname, buffer, n - 1);
            f->fname[n] = '\0';
        }

        if (n == -1)
        {
            perror("File read() in parse_file\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        f->child_count = 0;
        char buffer[36];
        uint32_t node_num;

        while ((n = read(fd, buffer, 36)) > 0)
        {
            memcpy(&node_num, buffer, sizeof(uint32_t));
            char entry_name[33];
            memcpy(entry_name, buffer + sizeof(uint32_t), 32);
            entry_name[32] = '\0';

            if (file_system[node_num] == NULL)
            {
                file_info *entry = malloc(sizeof(file_info));
                if (entry == NULL)
                {
                    perror("malloc()");
                    return EXIT_FAILURE;
                }

                entry->iNode = node_num;
                entry->fname[0] = '\0';
                file_system[node_num] = entry;
            }

            file_info *child_entry = malloc(sizeof(file_info));
            if (child_entry == NULL)
            {
                perror("malloc()");
                return EXIT_FAILURE;
            }

            strcpy(child_entry->fname, entry_name);
            child_entry->fname[32] = '\0';
            child_entry->iNode = node_num;

            if (node_num == f->iNode && strlen(f->fname) == 0)
            {
                strcpy(f->fname, entry_name);
            }
            f->children[f->child_count] = child_entry;
            f->child_count++;
        }

        if (n == -1)
        {
            perror("Directory read() in parse_file\n");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}