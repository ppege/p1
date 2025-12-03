#pragma once

int Scan_Plate(char plate_out[8]);

void BoxStart(int width);
void BoxLine(const char *text, int width);
void BoxBreak(int width);
void BoxEnd(int width);

void clear_screen();