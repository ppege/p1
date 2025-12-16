#pragma once

void box_start(int width);
void box_line(const char *text, int width);
void box_break(int width);
void box_end(int width);
int scan_plate(char plate_out[8]);
int validate_plate(const char *plate);
void clear_screen();

void box_line_start();
void box_line_fill(int printSize, int fillSize);
