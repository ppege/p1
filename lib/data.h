typedef enum {
  Standard,
  Handicap,
  Compact,
  EV
} SpaceType;

typedef struct {
  double x;
  double y;
  int level;
} Location;

typedef struct {
  double width;
  double height;
} Dimension;

const Dimension standardized_spaces[4] = {
  [Standard]  = {2.5, 5.0},
  [Handicap]  = {3.6, 5.0},
  [Compact]   = {2.3, 4.5},
  [EV]        = {2.6, 5.0},
}; // These might be magic numbers

const char* space_type_labels[4] = {"Standard", "Handicap", "Compact", "EV"};

typedef struct {
  SpaceType type;
  Location location;
} Space;

typedef struct {
  double x;
  double y;
} PathVector;

typedef struct {
  PathVector vector;
  Location start_point;
} Path;
