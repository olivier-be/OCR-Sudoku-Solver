#pragma once

void readhexadoku(const char* file, int** sudoku);
int check_line_hexa(int i, char digit, int** sudoku);
int check_colum_hexa(char digit, int j, int** sudoku);
int check_square_hexa(int i, int j, char digit, int** sudoku);
int IsValidHexadoku(int i, int j, char digit, int** sudoku);
int find_empty_cell_hexa(int* i, int* j, int** sudoku);
int solver_hexadoku(int** sudoku);
