#include <string.h>
#include <stdlib.h>

void rename_extension(char* filename, char* extension)
{
    char *dot = strrchr(filename, '.');

    if (dot != NULL) {
        // Replace the extension with a new one (e.g., ".new")
        strcpy(dot, extension);
    }
}