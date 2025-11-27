#include "PlateDB.h"
#include "data.h"
#include "lot.h"
#include "validate.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  // lot.c example
  Lot *lot = create_lot(2, 6, 60, 1, 1);
  populate_lot(lot);
  print_lot(lot);
  free_lot(lot);
  
  // lot file reading example
  Space space = {
      EV,
      {6.7, 4.2, 69},
  };
  char *FileName = "test/test.txt";
  int lines = GetFileLines(FileName);

  struct car *CarArr = (struct car *)malloc(sizeof(struct car) * lines);
  ReadFile(CarArr, lines, FileName);

  // Checking all the plates in the array
  for (int i = 0; i < lines; i++) {
    printf("Plate: %s type: %d\n", CarArr[i].plate, CarArr[i].carType);
  }

  // Testing the seaching function
  char plate[8] = "EZ69420";
  printf("Plate index: %d \n", GetCarIndexFromPlate(CarArr, lines, plate));

  printf("The type of the space is %d and the location is on x=%lf y=%lf and "
         "on floor %d\n",
         // space_type_labels[space.type],
         space.type, space.location.x, space.location.y, space.location.level);

  free(CarArr);
  
  // that's it
  return 0;
}
