#include "solver.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char** argv) {
    // Initialisation of the matrix
    int** sudoku = malloc(sizeof(int*) * 9);
    for (int i = 0; i < 9; i++) {
        sudoku[i] = malloc(sizeof(int) * 9);
    }
    if (ac == 0) {
        errx(1, "Not arg");
    }
    if (ac != 2) {
        errx(1, "Number of arg is false");
    }
    readsudo(argv[1], sudoku);
    if (solver_sudoku(sudoku)) {
        char* file = ".result";
        char* new = argv[1];
        // This function appends not more than n characters from the string
        // pointed to by src to the end of the string pointed to by dest
        // plus a terminating Null-character.
        strncat(new, file, 7);
        FILE* f = fopen(new, "w");
        if (f == NULL) {
            errx(1, "File don't create.");
        }
        for (size_t index = 0; index < 9; index++) {
            for (size_t index2 = 0; index2 < 9; index2++) {
                if (index2 != 0 && index2 % 3 == 0) {
                    // These functions writes a character string and an int
                    // character to a given file
                    fputs(" ", f); // end of the mini square
                }
                fputc(sudoku[index][index2] + 48, f);
            }
            fputs("\n", f);
            if (index != 0 && (index + 1) % 3 == 0) {
                fputs("\n", f);
            }
        }
    } else {
        printf("Sudoku is not resolved \n");
    }
    for (int i = 0; i < 9; i++) {
        free(sudoku[i]);
    }
    free(sudoku);
    return 0;
}
