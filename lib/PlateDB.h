#pragma once
#include <data.h>
// #include <stdio.h>

typedef struct {
  char plate[8];
  SpaceType carType;
} car;

car ReadLine(char line[10]);
int GetFileLines(char *FileName);
void ReadFile(car *CarArr, int lines, char *FileName);
int GetCarIndexFromPlate(car *CarArr, int size, char plate[8]);
