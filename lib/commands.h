#ifndef COMMANDS_H
#define COMMANDS_H

int ls(const int inode);
int mk_dir(const char *name);
int touch(const char *name);
int cd(const int inode);

#endif