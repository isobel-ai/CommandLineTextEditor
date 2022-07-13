# CommandLineTextEditor
An application that allows for creation, display and manipulation of text files.

This application is designed for use on Linux machines. To differentiate it from some of the text editors already used in Linux, all commands are typed as if the user was still operating in the main command line, rather than allowing the user to type directly into files and using control or alt-key sequences like in Nano. Moreover, as the user must start each instruction with a command, there’s no need for the text editor to have modes, which differentiates it from modal editors like Vim. 

When the text editor starts-up, a welcome message appears which introduces the user to the program and advises them to use the ‘help’ command (similar to the ‘info’ command in the Linux command-line) to access more information about it. The command works by reading from a hidden text file that stores information about the program’s operations. 

Each operation’s subroutine returns a flag (1 or 0) that the main program uses to determine whether to update the change log, which is implemented as a hidden text file.
