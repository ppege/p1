#include "overview.h"
#include "display.h"
#include "data.h"
#include <stdio.h>


//  Function designated to control the data that flows into printOverview
PathVector find_xy_minmax(const Lot *lot) {
    // Finds x_min, x_max, y_min, y_max for all objects that
    // has an x or y coordinate: (spaces, ups, downs, entrance, |maybe POI)
    double x_min = 0, y_min=0, x_max=0, y_max=0;
    for (int i = 0; i < lot->space_count; i++) {
        if (lot->spaces[i].location.x > x_max) {
            x_max = lot->spaces[i].location.x;
        }
        if (lot->spaces[i].location.y > y_max) {
            y_max = lot->spaces[i].location.y;
        }
    }
    // Finds the x_min, x_max, y_min, y_max values for all objects
    // that depends on a level count.
    for (int i = 0; i <= 1; i++) {
        if (lot->ups[i].x > x_max) {
            x_max = lot->ups->x;
        }
        if (lot->ups[i].y > y_max) {
            y_max = lot->ups->y;
        }
        if (lot->downs[i].x > x_max) {
            x_max = lot->downs->x;
        }
        if (lot->downs[i].y > y_max) {
            y_max = lot->downs->y;
        }
    }
    return (PathVector){x_max, y_max};
}



// The function responsible for printing the overview of the parking lot
void print_overview(Lot *lot) {
    // Brug af funktionen til at rengøre skærmen
    // clear_screen();
    Location my_entrance = lot -> entrance;
    printf("%lf %lf\n",my_entrance.x, my_entrance.y);

}