#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "lot.h"

int main() {
  Lot* lot = create_lot(6, 60, 1, 1);
  populate_lot(lot);
  print_lot(lot);
  free_lot(lot);
  return 0;
}

