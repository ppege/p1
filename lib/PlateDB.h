#pragma once
#include <data.h>
// #include <stdio.h>

struct car {
  char plate[8];
  SpaceType carType;
};

struct car ReadLine(char line[10]);
int GetFileLines(char *FileName);
void ReadFile(struct car *CarArr, int lines, char *FileName);
int GetCarIndexFromPlate(struct car *CarArr, int size, char plate[8]);
