#pragma once
#include <data.h>
// #include <stdio.h>

Car ReadLine(char line[10]);
int GetFileLines(char *FileName);
void ReadFile(Car *CarArr, int lines, char *FileName);
int GetCarIndexFromPlate(Car *CarArr, int size, char plate[8]);
