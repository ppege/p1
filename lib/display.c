#include "display.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Function to read license plate and check the data file
int Scan_Plate(char plate_out[8]) {
  char NumberPlate[8];

  printf("Enter your license plate: ");
  scanf("%s", NumberPlate);
  printf("Your license plate is: %s\n", NumberPlate);

  // If-statement to check if the license plate consists of 7 integers
  if (strlen(NumberPlate) !=7) {
  printf("Your license plate is not valid\n");
    return 1;
  }

  // Checking if the first two spots in the string is letters
  if (!isalpha(NumberPlate[0]) || !isalpha(NumberPlate[1])) {
    printf("Your license plate is not valid(First two must be letters)\n");
    return 1;
  }

  // Checking if the last five spots are digits
  for (int i = 2; i < 7; i++) {
    if (!isdigit(NumberPlate[i])) {
      printf("Your license plate is not valid (Last five must be digits)\n");
      return 1;
    }
  }

  // if everything is fine, the license plate is accepted.
  printf("Your numberplate is valid :): %s\n", NumberPlate);

  strcpy(plate_out, NumberPlate);
  return 0;
}

// Top of the box with corners.
void BoxStart(int width) {
  printf("╭");
  for (int i = 0; i < width; i++) printf("─");
  printf("╮\n");
}

// Creates the side part and text within the box
void BoxLine(const char *text, int width) {
  int len = strlen(text);
  int space = width - len;
  printf("│%s", text);
  for (int i = 0; i < space; i++) printf(" ");
  printf("│\n");
}

// Parts the lines from each other :)
void BoxBreak(int width) {
  printf("├");
  for (int i = 0; i < width; i++) printf("─");
  printf("┤\n");
}

//Ends the box
void BoxEnd(int width) {
  printf("╰");
  for (int i = 0; i < width; i++) printf("─");
  printf("╯\n");
}


