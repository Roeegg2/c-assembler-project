#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <file1> <file2>\n", argv[0]);
        return 1;
    }

    FILE *file1 = fopen(argv[1], "r");
    FILE *file2 = fopen(argv[2], "r");

    if (file1 == NULL || file2 == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    char line1[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];
    int lineNum = 0;
    int differenceFound = 0;

    while (1) {
        if (fgets(line1, MAX_LINE_LENGTH, file1) == NULL && fgets(line2, MAX_LINE_LENGTH, file2) == NULL) {
            break;
        }

        lineNum++;

        if (fgets(line2, MAX_LINE_LENGTH, file2) == NULL || strcmp(line1, line2) != 0) {
            printf("Difference found on line %d:\n", lineNum);
            printf("File 1: %s", line1);
            printf("File 2: %s", line2);
            differenceFound = 1;
            break;
        }
    }

    fclose(file1);
    fclose(file2);

    if (!differenceFound) {
        printf("Files are identical.\n");
    }

    return 0;
}
