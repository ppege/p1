#pragma once

typedef enum { Standard, Handicap, Compact, EV } SpaceType;

typedef struct {
  double x;
  double y;
  int level;
} Location;

typedef struct {
  double width;
  double height;
} Dimension;

static const Dimension standardized_spaces[4] = { // if this is not static we get MASSIVE ERRORS!!!
  [Standard]  = {2.5, 5.0},
  [Handicap]  = {3.6, 5.0},
  [Compact]   = {2.3, 4.5},
  [EV]        = {2.6, 5.0},
}; // These might be magic numbers

// const char* space_type_labels[4] = {"Standard", "Handicap", "Compact", "EV"};


typedef struct {
  SpaceType type;
  Location location;
  double rotation; // in degrees
  char* name;
  int occupied;
} Space;

typedef struct {
  double x;
  double y;
} Vector;

typedef struct {
  Vector corner[4];
} Rectangle;

typedef struct {
  Vector vector;
  Location start_point;
} Path;

static const double path_clearance = 1.5;
static const double path_accessibility = 6.0;

typedef struct {
  int level_count;
  Path* paths;
  int path_count;
  Space* spaces;
  int space_count;
  Location* ups;
  int up_count;
  Location* downs;
  int down_count;
  Location entrance;
  Location POI;
  double ramp_length;
} Lot;

Location get_endpoint(const Path path);
Rectangle get_space_rectangle(const Space space);
int compare_locations(Location loc1, Location loc2);
