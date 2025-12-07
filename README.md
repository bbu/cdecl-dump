cdecl-dump
==========

Dump C declarations visually on the command line.

## How to use

```
./cdecl-dump "int a"
./cdecl-dump "void f(int a)"
./cdecl-dump "unsigned char *const *arr[20][30]"
./cdecl-dump "int (*const fp[20])(void)"
```

## Building

 * `./build.sh` produces a debug build with additional tracing of the parsing stages
 * `DEBUG=0 ./build.sh` produces an optimised executable

## Bugs

 * The program doesn't do strict validation of the declarator in certain cases.
   Unlike the rules of C, it allows functions to return arrays and arrays to have functions as their elements.
 * Only built-in types are supported. For example, `size_t` or `uint64_t` will not be accepted.

## Screenshot

![alt tag](https://raw.github.com/bbu/cdecl-dump/master/screenshot.png)

## How it works

The program uses a hand-written, table-driven lexer and parser.
