#include "PlateDB.h"
#include "data.h"
#include "lot.h"
#include "lotReader.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  Lot *lot = create_lot(0, 10, 10, 10, 10);
  char *LotFileName = "test.lot";
  readLotFromFile(LotFileName, lot);

  print_lot(lot);

  char *FileName = "test/test.txt";

  // lot file reading example
  Space space = {
      EV,
      {6.7, 4.2, 69},
  };
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
