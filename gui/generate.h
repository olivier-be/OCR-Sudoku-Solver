#pragma once

void add_number(
    guchar* final_img, int number, int x, int y, char mask[], int color);
GdkPixbuf* initialization(char* grid_path, int** grid);
GdkPixbuf* generate(char* grid_path, int** grid);
guchar* get_not_solved_img();
