#include <stdio.h>
#include "lot.h"
#include "image.h"
#include "lotReader.h"
#include "validate.h"

int main() {
  Lot *lot = create_lot(1, 1, 1, 1, 1);
  char *LotFileName = "diagonal.lot";
  readLotFromFile(LotFileName, lot);
  ValidationResult result = validate_lot(lot);

  if (result.result == Err) {
    printf("Lot validation failed! %s\n", validation_error_message(result.error));
    free_lot(lot);
    return 1;
  }

  lot_to_ppm_all_levels(lot, "parking", 100);

  free_lot(lot);
  return 0;
}
