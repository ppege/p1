#include "overview.h"
#include "display.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <tgmath.h>

// Need some sort of function to handle print logic for
// how paths or parking spots show up.

//  Function that finds upper bound for parking lot
Vector find_xy_minmax(const Lot *lot, const int level) {
    // Finds x_min, x_max, y_min, y_max for all objects that
    // has an x or y coordinate: (spaces, ups, downs, entrance, |maybe POI)
    // for a specific level.
    double x_max=0, y_max=0;
    for (int i = 0; i < lot->space_count; i++) {
        if (lot->spaces[i].location.level > level) {
            break;
        }
        if (lot->spaces[i].location.x > x_max && lot->spaces[i].location.level == level) {
            x_max = lot->spaces[i].location.x;
        }
        if (lot->spaces[i].location.y > y_max && lot->spaces[i].location.level == level) {
            y_max = lot->spaces[i].location.y;
        }

    }
    // Finds the x_min, x_max, y_min, y_max values for all objects
    // that depends on a level count.
    for (int i = 0; i <= 1; i++) {
        if (lot->ups[i].x > x_max && lot->ups[i].level == level) {
            x_max = lot->ups[i].x;
        }
        if (lot->ups[i].y > y_max && lot->ups[i].level == level) {
            y_max = lot->ups[i].y;
        }
        if (lot->downs[i].x > x_max && lot->downs[i].level == level) {
            x_max = lot->downs[i].x;
        }
        if (lot->downs[i].y > y_max && lot->downs[i].level == level) {
            y_max = lot->downs[i].y;
        }
    }
    return (Vector){ceil(x_max), ceil(y_max)};
}

// Function to translate coords into the correct elements in the array;
int cord_calc(const int x, const int y, const Lot *lot, const int level) {
    const int width = o_width(lot, level);
    const int height = o_height(lot, level);
    int x_value = 0, y_value = 0;

    // Magic numbers:
    // 4 is the padding
    // 2 is to move it over the padding
    if (x < width-4) {
        x_value = x+2;
    }

    if (y < height-4) {
        y_value= y+2;
    }

    //printf("y = %d x = %d", y_value, x_value);

    return y_value*width+x_value;
}

// Calculates the height for the array, so it includes
// parking + padding;
int o_height(const Lot *lot, const int level) {
    const Vector max = find_xy_minmax(lot, level);
    return (2*(int)max.y + 2);
}

// Calculates the width for the array, so it includes
// parking + padding;
int o_width(const Lot *lot, const int level) {
    const Vector max = find_xy_minmax(lot, level);
    return (2*(int)max.x + 2);
}


// Function to insert a parking spot placed by lower x and y value
void insert_parking_spot(const Lot *lot, char** populated_arrays, const int level) {


}

// Make dynamic array for each level
// Function to create a dynamic array for a given level
char* populate_array(const Lot *lot, const int level) {
    const int height = o_height(lot,level);
    const int width = o_width(lot, level);
    const int size_of_lot = (width*height);
    char** parking_lot = calloc(size_of_lot, sizeof(char));

    // For loop that creates the border around the parking lot
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // Goes to the next iteration of the loop
            // When the values don't match the ones in the border
            if ((j>=2 && j<=width-3) && (i>=2 && i<=height-3)) {
                (*parking_lot)[width*i+j] = 'x';
                continue;
            }
            // Creates the actual border
            if (j == 0 || i == 0 || j == width-1 || i == height-1) {
                (*parking_lot)[width*i+j] = '#';
            }
            // Pads the border at the bottom and top
            if (j >= 1 && j <= width-2 && (i == 1 || i == height-2)) {
                *parking_lot[width*i+j] = ' ';
            }
            // Pads the border at both sides
            if (i >= 1 && i <= height-2 && (j == 1 || j == width-2)) {
                *parking_lot[width*i+j] = ' ';
            }
        }
    }
    // For loop that makes the lower and upper bound
    *parking_lot[cord_calc(0,0,lot,0)] = 'f';
    return *parking_lot;
}

// Creates an array of pointers, so it is easy to acces the overview
// for each level.
char** populated_arrays(const Lot *lot) {
    const int levels = lot->level_count;
    char** levels_overview = malloc(levels * sizeof(char*));
    for (int i = 0; i < levels; i++) {
        levels_overview[i] = populate_array(lot, i);
    }
    return levels_overview;
}

// The function responsible for printing the overview of the parking lot
void print_overview(const Lot *lot, const char* parking_lot, const int level) {
    // Brug af funktionen til at rengøre skærmen
    // clear_screen();
    /*Location my_entrance = lot -> entrance;
    printf("%lf %lf\n",my_entrance.x, my_entrance.y);
    */
    const int height = o_height(lot, level);
    const int width = o_width(lot, level);
    int wrap = width*height-width;
    clear_screen();
    // Loop that defines how tall the parking space is:
    for (int i = 0; i <= (height); i++) {
        for (int j = 0; j < (width); j++) {
            if (j >= 0 && j < width && i >= 0 && i < height) {
                printf("%c", parking_lot[wrap+j]);
            }
        }
        wrap -=  width;
        printf("\n");
    }
}

// Frees the allocated memory
void overview_free(const Lot *lot, char** populated_arrays) {
    const int levels = lot->level_count;
    for (int i = 0; i < levels; i++) {
        free(populated_arrays[i]);
    }
    free(populated_arrays);
}

