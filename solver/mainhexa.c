#include "hexa.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** argv) {
    int** sudoku = malloc(sizeof(int*) * 16);
    for (int i = 0; i < 16; i++) {
        sudoku[i] = malloc(sizeof(int) * 16);
    }
    if (ac == 0) {
        errx(1, "Not arg");
    }
    if (ac != 2) {
        errx(1, "Number of arg is false");
    }
    readhexadoku(argv[1], sudoku);
    if (solver_hexadoku(sudoku)) {
        char* file = ".result";
        char* new = argv[1];
        strncat(new, file, 7);
        FILE* f = fopen(new, "w");
        if (f == NULL) {
            errx(1, "File don't create.");
        }
        for (size_t index = 0; index < 16; index++) {
            for (size_t index2 = 0; index2 < 16; index2++) {
                if (index2 != 0 && index2 % 4 == 0) {
                    fputs(" ", f);
                }

                if (sudoku[index][index2] >= 0xA
                    && sudoku[index][index2] <= 0xF) {
                    fputc(sudoku[index][index2] + 'A' - 0xA, f);
                } else {
                    fputc(sudoku[index][index2] + 48, f);
                }
            }
            fputs("\n", f);
            if (index != 0 && (index + 1) % 4 == 0) {
                fputs("\n", f);
            }
        }
    } else {
        printf("Sudoku is not resolved \n");
    }
    for (int i = 0; i < 16; i++) {
        free(sudoku[i]);
    }
    free(sudoku);
    return 0;
}
