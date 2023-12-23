#pragma once

void add_number_hexa(
    guchar* final_img, char number, int x, int y, char mask[], int color);
GdkPixbuf* initialization_hexa(const char* grid_path);
GdkPixbuf* generate_hexa(const char* grid_path);
guchar* get_not_solved_hexa_img();
