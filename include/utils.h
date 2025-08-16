#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define MAX_LENGTH 64 // Maximum buffer length for reading lines

// Reads a line from a file into buffer
char *read_line(FILE *file, char *buffer, int size);

#endif