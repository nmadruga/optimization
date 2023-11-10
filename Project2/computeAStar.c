#include <stdio.h>
#include <string.h>

int main() {
    FILE *file;
    char str_to_write[] = "Hello, Binary File!";
    char str_to_read[100]; // Make sure it's large enough to hold the string.

    // Write the string to a binary file
    file = fopen("binaryfile.bin", "wb");
    if (file == NULL) {
        printf("Failed to open the file for writing.\n");
        return 1;
    }

    fwrite(str_to_write, sizeof(char), strlen(str_to_write), file);

    fclose(file);

    // Read the string from the binary file
    file = fopen("binaryfile.bin", "rb");
    if (file == NULL) {
        printf("Failed to open the file for reading.\n");
        return 1;
    }

    fread(str_to_read, sizeof(char), sizeof(str_to_read), file);
    fclose(file);

    printf("Read from file: %s\n", str_to_read);

    return 0;
}
