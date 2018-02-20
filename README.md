# Grep
Custom Grep with system calls


UNIX Grep written in the C programming language using system calls for file I/O (open(), read(), and close()). No buffered I/O library functions such as fopen(), fread(), etc were used. Here is the command line format for usfgrep:

USAGE:
usfgrep <string> <file1> [<file2> ...]


	
$ usfgrep main *.c
args.c:20: int main(int argc, char **argv) {
count.c:15: int main(int argc, char **argv) {
