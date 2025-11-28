#include "overview.h"
#include "display.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

// Need some sort of function to handle vectors?

//  Function that finds upper bound for parking lot
Vector find_xy_minmax(const Lot *lot) {
    // Finds x_min, x_max, y_min, y_max for all objects that
    // has an x or y coordinate: (spaces, ups, downs, entrance, |maybe POI)
    double x_max=0, y_max=0;
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
    return (Vector){ceil(x_max), ceil(y_max)};
}

// Calculates the height for the array, so it includes
// parking + padding;
int o_height(const Lot *lot, Vector xy) {
    const Vector max = find_xy_minmax(lot);
    return (int)(2*max.y+4);
}

// Calculates the width for the array, so it includes
// parking + padding;
int o_width(const Lot *lot, Vector xy) {
    const Vector max = find_xy_minmax(lot);
    return (int)(2*max.x+4);
}

// Function that creates the first array
char* populate_array(const Lot *lot) {
    const int height = o_height(lot, find_xy_minmax(lot));
    const int width = o_width(lot, find_xy_minmax(lot));
    const int size_of_lot = (width*height);
    char* parking_lot = malloc(size_of_lot);

    // For loop that creates the border around the parking lot
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Goes to the next iteration of the loop
            // When the values don't match the ones in the border
            if ((j>=2 && j<=width-3) && (i>=2 && i<=height-3)) {
                parking_lot[width*i+j] = 'x';
                continue;
            }
            // Creates the actual border
            if (j == 0 || i == 0 || j == width-1 || i == height-1) {
                parking_lot[width*i+j] = '#';
            }
            // Pads the border at the bottom and top
            if (j >= 1 && j <= width-2 && (i == 1 || i == height-2)) {
                parking_lot[width*i+j] = ' ';
            }
            // Pads the border at both sides
            if (i >= 1 && i <= height-2 && (j == 1 || j == width-2)) {
                parking_lot[width*i+j] = ' ';
            }
        }
    }
    // For loop that makes the lower and upper bound
    return parking_lot;
}

/*
// Function to insert a parking spot placed by lower x and y value
void insert_parkingspot(const Lot *lot, char* parking_lot) {


    Array logic
}
*/

// The function responsible for printing the overview of the parking lot
void print_overview(const Lot *lot, const char* parking_lot) {
    // Brug af funktionen til at rengøre skærmen
    // clear_screen();
    /*Location my_entrance = lot -> entrance;
    printf("%lf %lf\n",my_entrance.x, my_entrance.y);
    */
    const int height = o_height(lot, find_xy_minmax(lot));
    const int width = o_width(lot, find_xy_minmax(lot));
    int wrap = width*height-width;
    // Loop that defines how tall the parking space is:
    for (int i = 0; i < (height); i++) {
        for (int j = 0; j < (width); j++) {
            if (j != 0 || j != width || i != 0 || i != height) {
                printf("%c", parking_lot[wrap+j]);
            }
        }
        wrap -=  width;
        printf("\n");
    }
}

void overview_free(char* parking_lot) {
    free(parking_lot);
}
