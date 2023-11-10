#pragma once

void readsudo(char* file, int** sudoku);
int check_line(int i, int digit, int** sudoku);
int check_colum(int digit, int j, int** sudoku);
int check_square(int i, int j, int digit, int** sudoku);
int IsValidSudoku(int i, int j, int digit, int** sudoku);
int solver_sudoku(int** sudoku);
