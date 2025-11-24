#include <stdio.h>
#include "data.h"

int main() {
  Space space = {
    EV,
    {6.7, 4.2, 69},
  };
  printf("Hello World!\n");
  printf(
    "The type of the space is %d and the location is on x=%lf y=%lf and on floor %d\n",
    //space_type_labels[space.type],
    space.type,
    space.location.x,
    space.location.y,
    space.location.level
  );
  return 0;
}

