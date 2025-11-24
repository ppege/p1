#include "PlateDB.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  Space space = {
      EV,
      {6.7, 4.2, 69},
  };
  char *FileName = "test.txt";
  int lines = GetFileLines(FileName);

  printf("AHAHHAHA\n");
  printf("Lines in txt: %d\n", lines);
  // fclose(fptr);
  // fptr = fopen("test.txt", "r");
  struct car *CarArr = (struct car *)malloc(sizeof(struct car) * lines);
  ReadFile(CarArr, lines, FileName);

  for (int i = 0; i < lines; i++) {
    printf("Plate: %s type: %d\n", CarArr[i].plate, CarArr[i].carType);
  }

  char plate[8] = "EZ69420";
  printf("Plate index: %d \n", GetCarIndexFromPlate(CarArr, lines, plate));
  // struct car tempCar = ReadLine(line);
  // printf("Plate: %s num: %d \n", tempCar.plate, tempCar.carType);

  printf("The type of the space is %d and the location is on x=%lf y=%lf and "
         "on floor %d\n",
         // space_type_labels[space.type],
         space.type, space.location.x, space.location.y, space.location.level);

  free(CarArr);
  return 0;
}
