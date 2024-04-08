# Simple Inode
This is a program that simulates a simplification of the inode used in unix style file systems.

# Running
compile with
`cd simple-inode`
`gcc *.c -o sinode`
run
`./sinode`

# Usage
The program will interact with onde or more text files and structure them in memory using a BTree.
Each node is 10 characters long. Every node is stored in the BTree.
after running `./sinode` the shell will appear:

you can use `help` to show the list of commands.

to create a file:
`add <filename>`
ex: `add hammer.txt`
after creating the file, it will prompt for its contents

printing the contents of a file:
`print <filename>`
ex: `print hammer.txt`

search for a string in the files:
`search <text>`
ex: `search 123`


