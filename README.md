File System Emulator

This program emulates a simplified file system using actual files and directories on your machine. It supports creating files and directories, navigating between directories, and listing their contents.

Running the Emulator

Compile with: make

This will produce an executable named fs_emulator.

Run the emulator by passing in a directory that contains an inodes_list file (e.g., the provided fs or empty directories): ./fs_emulator fs

Commands:

    Once running, the emulator starts in the root directory (inode 0). You can enter commands interactively:

    ls
    Lists the contents of the current directory.

    ls <name>
    Lists the contents of a specified subdirectory by name.

    cd <name>
    Change the current directory to the specified one. Only single-level names are supported (e.g., cd lab2, not cd lab2/task1).

    mkdir <name>
    Create a new directory in the current directory. Names are limited to 32 characters.

    touch <name>
    Create a new file in the current directory. The file’s name is written into the actual file that represents it.

    exit (or press Ctrl+D)
    Save the current state of the inode list and quit.

Notes:

The emulator supports up to 1024 inodes (0–1023).

Always run the program inside a copy of the provided empty or fs directories so you don’t corrupt the originals.

On exit, the inodes_list file will be updated to reflect your changes.
