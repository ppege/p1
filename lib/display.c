#include "display.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Top of the box with corners.
void box_start(int width) {
  printf("╭");
  for (int i = 0; i < width; i++)
    printf("─");
  printf("╮\n");
}

void box_line_start() { printf("│"); }

void box_line_fill(int printSize, int fillSize) {
  int size = fillSize - printSize;
  for (int i = 0; i < size; i++)
    printf(" ");
  printf("│\n");
}

// Creates the side part and text within the box
void box_line(const char *text, int width) {
  int len = strlen(text);
  int space = width - len;
  printf("│%s", text);
  for (int i = 0; i < space; i++)
    printf(" ");
  printf("│\n");
}

// Parts the lines from each other :)
void box_break(int width) {
  printf("├");
  for (int i = 0; i < width; i++)
    printf("─");
  printf("┤\n");
}

// Ends the box
void box_end(int width) {
  printf("╰");
  for (int i = 0; i < width; i++)
    printf("─");
  printf("╯\n");
}

// Validates a plate string, returns 0 if valid, 1 if invalid
int validate_plate(const char *plate) {
  if (strlen(plate) != 7) {
    return 1;
  }

  if (!isalpha(plate[0]) || !isalpha(plate[1])) {
    return 1;
  }

  for (int i = 2; i < 7; i++) {
    if (!isdigit(plate[i])) {
      return 1;
    }
  }

  return 0;
}

// function to scan and validate a license plate
int scan_plate(char plate_out[8]) {
  char NumberPlate[8];

  printf("Enter your license plate: ");
  scanf("%7s", NumberPlate); // FIXED: limit input to 7 chars
  // printf("Your license plate is: %s\n", NumberPlate);

  if (validate_plate(NumberPlate) != 0) {
    printf("Your license plate is not valid\n");
    return 1;
  }

  // printf("Your numberplate is valid :): %s\n", NumberPlate);
  strcpy(plate_out, NumberPlate);
  return 0;
}

void clear_screen() { printf("\033[1;1H\033[2J"); }
