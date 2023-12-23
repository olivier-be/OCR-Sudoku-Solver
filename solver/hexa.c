#include "hexa.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void readhexadoku(const char* file, int** sudoku) {
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        errx(1, "Not open a file %s", file);
    }
    const unsigned LENGTH_LIMIT = 256;
    char buffer[LENGTH_LIMIT];
    int l = 0;
    int c = 0;
    int z = 3;
    while (fgets(buffer, LENGTH_LIMIT, f) || z > 0) {
        if (buffer[0] == '\n') {
            z -= 1;
        } else {
            for (size_t index = 0; index < strlen(buffer); index++) {
                if (buffer[index] >= '0' && buffer[index] <= '9') {
                    sudoku[l][c] = buffer[index] - '0';
                    c += 1;
                }
                if (buffer[index] >= 'A'
                    && buffer[index]
                        <= 'F') { /*recup le nombre correspondant a la lettre (A
                                     = 10, B = 11, ect...)*/
                    sudoku[l][c] = buffer[index] - 55;
                    c += 1;
                }
                if (buffer[index] == '.') {
                    sudoku[l][c] = -1;
                    c += 1;
                }
            }
            c = 0;
            l += 1;
        }
    }
    fclose(f);
}

int check_line_hexa(int i, char digit, int** sudoku) {
    int k = 0;
    while (k < 16) {
        if (sudoku[i][k] == digit) {
            return 0;
        }
        k += 1;
    }
    return 1;
}

int check_colum_hexa(char digit, int j, int** sudoku) {
    int k = 0;
    while (k < 16) {
        if (sudoku[k][j] == digit) {
            return 0;
        }
        k += 1;
    }
    return 1;
}

int check_square_hexa(int i, int j, char digit, int** sudoku) {
    int l = 4 * (i / 4);
    int c = 4 * (j / 4);
    int k = 0;
    while (k < 16) {
        if (sudoku[l + (k % 4)][c + (k / 4)] == digit) {
            return 0;
        }
        k += 1;
    }
    return 1;
}

int IsValidHexadoku(int i, int j, char digit, int** sudoku) {
    if (check_line_hexa(i, digit, sudoku) && check_colum_hexa(digit, j, sudoku)
        && check_square_hexa(i, j, digit, sudoku)) {
        return 1;
    } else {
        return 0;
    }
}

int find_empty_cell_hexa(int* i, int* j, int** sudoku)
// This function is used to check whether a cell is free in order to enter a
// number.
{
    for (int i2 = 0; i2 < 16; i2++) {
        for (int j2 = 0; j2 < 16; j2++) {
            if (-1 == sudoku[i2][j2]) {
                *i = i2;
                *j = j2;
                return 1;
            }
        }
    }
    return 0;
}

int solver_hexadoku(int** sudoku) {
    int l;
    int c;
    if (!find_empty_cell_hexa(&l, &c, sudoku)) {
        return 1;
    }
    char digit = 0;
    while (digit <= 15) {
        if (IsValidHexadoku(l, c, digit, sudoku)) {
            sudoku[l][c] = digit;
            if (solver_hexadoku(sudoku)) {
                return 1;
            }
            sudoku[l][c] = -1;
        }
        digit += 1;
    }
    return 0;
}
