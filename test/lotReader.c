#include "lotReader.h"
#include "data.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

void test_lot_from_file() {
  Lot lot = lot_from_file("../../test/test.lot");
  TEST_ASSERT_EQUAL_INT(24, lot.space_count);
  TEST_ASSERT_EQUAL_INT(7, lot.path_count);
  TEST_ASSERT_EQUAL_INT(2, lot.level_count);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, lot.entrance.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, lot.entrance.y);
}

void test_readSpace() {
  char *line = "name=A1 type=2 location(x=-12.0 y=2.0 level=0) rotation=0";
  Space *space = readSpace(line);
  if (!space) return;
  TEST_ASSERT_EQUAL_STRING("A1", space->name);
  TEST_ASSERT_EQUAL_INT(2, space->type);
  TEST_ASSERT_FLOAT_WITHIN(0.001, -12.0, space->location.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 2.0, space->location.y);
  TEST_ASSERT_EQUAL_INT(0, space->location.level);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, space->rotation);
}

void test_readPath() {
  char *line = "vec(x=1.0 y=0.0) location(x=0.0 y=0.0 level=0)";
  Path *path = readPath(line);
  if (!path) return;
  TEST_ASSERT_FLOAT_WITHIN(0.001, 1.0, path->vector.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, path->vector.y);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, path->start_point.x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, path->start_point.y);
  TEST_ASSERT_EQUAL_INT(0, path->start_point.level);
}

void test_readLocation() {
  char *line = "x=6.7 y=20.0 level=5";
  Location *location = readLocation(line);
  if (!location) return;
  TEST_ASSERT_FLOAT_WITHIN(0.001, 6.7, location->x);
  TEST_ASSERT_FLOAT_WITHIN(0.001, 20.0, location->y);
  TEST_ASSERT_EQUAL_INT(5, location->level);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lot_from_file);
  RUN_TEST(test_readSpace);
  RUN_TEST(test_readPath);
  RUN_TEST(test_readLocation);
  return UNITY_END();
}
