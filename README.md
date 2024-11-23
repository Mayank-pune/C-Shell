# C Shell

A shell program written in C which supports basic bash commands with proper error-handling.

## Commands
Following are the list of commands implemented in the shell.

- `pwd` : prints the present working directory
- `echo` : prints a text in the command line
- `history` : Prints the last 10 most recent commands given (in the same order)
- `cd` : changes directory
- `exit` : exits from the shell
 

 ## Code Structure
 The code is kept quite modular for easy debugging re-usability, and modification. Each task is done in a function. Following are the file discriptions.
 
 - `colors.h` : Contains the ANSI codes for colors used in the shell
 - `header.h` : Contains the header of all the functions used in the program, to avoid implicit declaration.
 - `history.txt` : A simple text file that contains the history of commands (with a capacity of 20 : can be changed)
 - `shell.c` : Contains the main code and the functions used in the program
 - `Makefile` : Compiles `shell.c` and runs the executable `shell`. Also allows to clear the executable. Following are the possible commands and their descriptions.
    - `make` - compiles the `shell.c` file and create an executable named `shell`
    - `make run` - This will compile the shell program (if not already compiled) and run it. After the program finishes, it will display "Shell ended".
    - `make clean` - This will remove the compiled shell executable from the directory.

    We can directly use `make run` to start the shell.



 