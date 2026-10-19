#include <stdio.h>
#include <stdlib.h>
#include "files.h"

char *read_file(const char *filename, long * out_size)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    *out_size = ftell(f);
    rewind(f);

    if (*out_size < 0) {
        fclose(f);
        return NULL;
    }

    char *buffer = (char *) malloc(*out_size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, *out_size, f);
    fclose(f);

    if (bytes_read != (size_t)*out_size) {
        free(buffer);
        return NULL;
    }

    buffer[*out_size] = '\0';
    return buffer;
}