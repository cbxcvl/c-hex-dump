# c-hex-dump

My C implementation of a hex viewer inspired by the default output of **xxd**. The executable is named **hexview**. It takes a file path and displays the file offset, hexadecimal bytes, and a text representation on each line.

I built this project to practice binary file reading, arrays, offsets, output formatting, and error handling in C.

## Features

- Reads binary files, including `0x00` and `0xFF` bytes.
- Displays 16 bytes per line, grouped into hexadecimal pairs.
- Shows hexadecimal offsets and keeps the text column aligned on short final lines.
- Replaces non-printable bytes with `.` in the text column.
- Colors bytes when output goes to a compatible terminal. Redirected output contains no color codes, and a non-empty `NO_COLOR` value disables colors.
- Writes errors to standard error and returns a nonzero status for invalid arguments or file open/read failures.

## Requirements

- Linux or another POSIX environment.
- A C compiler such as GCC or Clang.

The project uses the C standard library and the POSIX `isatty` function. No third-party libraries are required.

## Build

~~~sh
gcc -std=c11 -Wall -Wextra -Wpedantic -o hexview main.c
~~~

## Usage

~~~sh
./hexview path/to/file
~~~

Reproducible example:

~~~sh
printf 'Hello' > example.bin
./hexview example.bin
~~~

Output:

~~~text
00000000: 4865 6c6c 6f                             Hello
~~~

To save output without color codes:

~~~sh
./hexview example.bin > dump.txt
~~~

To disable colors in the terminal:

~~~sh
env NO_COLOR=1 ./hexview example.bin
~~~

## How it works

The program reads one byte at a time with `fgetc`. It stores the return value in an `int` so it can distinguish every valid byte from `EOF`. Bytes are collected in a 16-byte array and printed as full lines or, at the end of the file, a partial line.

The text column uses `isprint` to decide which bytes can be displayed as characters. When color is available, ANSI escape sequences highlight byte values without changing redirected output.

## Scope

This version implements the default hexadecimal view for a file supplied on the command line. Additional xxd features, such as reversing a dump, reading standard input, and choosing the number of columns, are not implemented.
