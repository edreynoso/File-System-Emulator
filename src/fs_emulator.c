#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdbool.h>
#include "helper.h"
#include "commands.h"

int main(int argc, char **argv)
{
    if (chdir(argv[1]) == -1)
    {
        printf("Could not change directories in main\n");
        return EXIT_FAILURE;
    }

    int fd = open("inodes_list", O_RDONLY);

    if (fd == -1)
    {
        printf("Could not open inodes_list\n");
        return EXIT_FAILURE;
    }
    char buffer[sizeof(uint32_t) + sizeof(char)];

    while (read(fd, buffer, 5) == 5)
    {
        char ft = buffer[4];
        int node_num;
        memcpy(&node_num, buffer, sizeof(int));
        if ((node_num == 0) && (ft != 'd'))
        {
            printf("The root node is not a directory");
            exit(EXIT_FAILURE);
        }

        if (node_num > 1023 || node_num < 0)
            continue;

        file_info *current;

        if (file_system[node_num] == NULL)
        {
            current = malloc(sizeof(file_info));
            if (current == NULL)
            {
                perror("malloc()");
                return EXIT_FAILURE;
            }
            current->iNode = node_num;
        }
        else
        {
            current = file_system[node_num];
        }
        if (ft == 'f')
            current->file_type = file;
        else if (ft == 'd')
            current->file_type = directory;
        else
            continue;

        char *name = uint32_to_str(node_num);
        file_system[node_num] = current;
        parse_file(name, current);

        free(name);
    }

    if (file_system[0]->file_type != directory)
    {
        printf("The root node is not a directory\n");
        exit(EXIT_FAILURE);
    }

    char *commands = NULL;
    size_t buf_size = 0;

    ssize_t check;

    int capacity = 2;
    char *args[3];

    while (printf("> ") != EOF && (check = getline(&commands, &buf_size, stdin)) != EOF)
    {
        if (commands[check - 1] == '\n')
        {
            commands[check - 1] = '\0';
        }
        int count = 0;

        char *token = strtok(commands, " ");
        while (token != NULL)
        {
            if (count > capacity)
            {
                break;
            }
            args[count] = token;
            token = strtok(NULL, " ");
            count++;
        }
        args[2] = NULL;
        if (count == 2)
        {
            if (strcmp(args[0], "cd") == 0)
            {
                char name[33];
                memcpy(name, args[1], 32);
                name[32] = '\0';
                int inode = get_directory_inode(name);
                if (inode == -1)
                {
                    printf("This directory does not exit\n");
                }
                else
                {
                    cd(inode);
                }
            }
            else if (strcmp(args[0], "ls") == 0)
            {
                char name[33];
                memcpy(name, args[1], 32);
                name[32] = '\0';
                int inode = get_directory_inode(name);
                if (inode == -1)
                {
                    printf("This directory does not exit\n");
                }
                else
                {
                    ls(inode);
                }
            }
            else if (strcmp(args[0], "mkdir") == 0)
            {
                char name[33];
                memcpy(name, args[1], 32);
                name[32] = '\0';
                if (mk_dir(name) == EXIT_FAILURE)
                    printf("There is alread a directory with the name: %s\n", name);
            }
            else if (strcmp(args[0], "touch") == 0)
            {
                char name[33];
                memcpy(name, args[1], 32);
                name[32] = '\0';
                if (touch(name) == EXIT_FAILURE)
                    printf("There is alread a file with the name: %s\n", name);
            }
            else
            {
                printf("Please type a valid command\n");
            }
        }
        else
        {
            if (strcmp(args[0], "ls") == 0)
            {
                ls(current_directory);
            }
            else if (strcmp(args[0], "cd") == 0)
            {
                printf("Plese provide a directory!\n");
            }
            else
            {
                printf("Please type a valid command\n");
            }
        }
    }
    exit_program(argv[1]);
    free(commands);
    close(fd);
    return 0;
}