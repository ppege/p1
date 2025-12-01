#include "lotReader.h"
#include "data.h"
#include "unity.h"
#include <stdio.h>

void setUp() {}

void tearDown() {}

void test_read_path(void) {
  char line[] = "vec(x=12.0 y=0.0) location(x=1.0 y=2.0 level=0)";
  Path path = readPath(line);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 12.0, path.vector.x);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 0.0, path.vector.y);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 1.0, path.start_point.x);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 2.0, path.start_point.y);
  TEST_ASSERT_EQUAL_INT(0, path.start_point.level);
}

void test_read_location(void) {
  char line[] = "x=2.5 y=1.25 level=0";
  Location location = readLocation(line);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 2.5, location.x);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 1.25, location.y);
  TEST_ASSERT_EQUAL_INT(0, location.level);
}

void test_read_space(void) {
  char line[] = "name=A1 type=0 location(x=1.0 y=2.5 level=0) rotation=0.0";
  Space space = readSpace(line);
  TEST_ASSERT_EQUAL_STRING("A1", space.name);
  TEST_ASSERT_EQUAL_INT(0, space.type);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 1.0, space.location.x);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 2.5, space.location.y);
  TEST_ASSERT_EQUAL_INT(0, space.location.level);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 0.0, space.rotation);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_read_path);
  RUN_TEST(test_read_location);
  RUN_TEST(test_read_space);
  return UNITY_END();
}
