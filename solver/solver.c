#include "solver.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void readsudo(char* file, int** sudoku)
// This function allows you to read and save the sudoku from a file
{
    FILE* f = fopen(file, "r");
    if (f == NULL) {
        errx(1, "Not open a file %s", file);
    }
    const unsigned LENGTH_LIMIT = 256;
    char buffer[LENGTH_LIMIT];
    int l = 0;
    int c = 0;
    int z = 2;
    while (fgets(buffer, LENGTH_LIMIT, f) || z > 0) {
        if (buffer[0] == '\n') {
            z -= 1;
        } else {
            for (size_t index = 0; index < strlen(buffer); index++) {
                if (buffer[index] > '0' && buffer[index] <= '9') {
                    sudoku[l][c] = buffer[index] - '0';
                    c += 1;
                } else if (buffer[index] == '.') {
                    sudoku[l][c] = 0;
                    c += 1;
                }
            }
            c = 0;
            l += 1;
        }
    }
    fclose(f);
}

int check_line(int i, int digit, int** sudoku)
// check if a number is valid on the line
{
    int k = 0;
    while (k < 9) {
        if (sudoku[i][k] == digit) {
            return 0;
        }
        k++;
    }
    return 1;
}

int check_colum(int digit, int j, int** sudoku)
// check if a number is valid on the colum
{
    int k = 0;
    while (k < 9) {
        if (sudoku[k][j] == digit) {
            return 0;
        }
        k++;
    }
    return 1;
}

int check_square(int i, int j, int digit, int** sudoku)
// check if a number is valid on the square
{
    int l = 3 * (i / 3);
    int c = 3 * (j / 3);
    int k = 0;
    while (k < 9) {
        if (sudoku[l + (k % 3)][c + (k / 3)] == digit) {
            return 0;
        }
        k++;
    }
    return 1;
}

int IsValidSudoku(int i, int j, int digit, int** sudoku)
// check if the sudoku is valid for the filling
{
    if (check_line(i, digit, sudoku) && check_colum(digit, j, sudoku)
        && check_square(i, j, digit, sudoku)) {
        return 1;
    } else {
        return 0;
    }
}

int find_empty_cell(int* i, int* j, int** sudoku)
// This function is used to check whether a cell is free in order to enter a
// number.
{
    for (int i2 = 0; i2 < 9; i2++) {
        for (int j2 = 0; j2 < 9; j2++) {
            if (!sudoku[i2][j2]) {
                *i = i2;
                *j = j2;
                return 1;
            }
        }
    }
    return 0;
}

int solver_sudoku(int** sudoku)
// This function is the solver algorithm. It solves the sudoku by filling in
// the missing numbers.
{
    int l;
    int c;
    if (!find_empty_cell(&l, &c, sudoku)) {
        return 1;
    }
    int digit = 1;
    while (digit <= 9) {
        if (IsValidSudoku(l, c, digit, sudoku)) {
            sudoku[l][c] = digit;
            if (solver_sudoku(sudoku)) {
                return 1;
            }
            sudoku[l][c] = 0;
        }
        digit++;
    }
    return 0;
}
