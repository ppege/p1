#include "unity.h"
#include "data.h"
#include "calculations.h"
#include <stdio.h>

void setUp() {}

void tearDown() {}

void test_get_endpoint(void) {
  Path path = {
    .vector = {3.0, 4.0},
    .start_point = {1.0, 2.0, 0}
  };
  Location endpoint = get_endpoint(&path);
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(4.0, endpoint.x, "get_endpoint should have found the correct endpoint x");
  TEST_ASSERT_EQUAL_FLOAT_MESSAGE(6.0, endpoint.y, "get_endpoint should have found the correct endpoint y");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, endpoint.level, "get_endpoint should have found the correct endpoint level");
}

void test_get_space_rectangle(void) {
  Space space = {
    .type = Standard,
    .location = {10.0, 5.0, 0},
    .rotation = 45.0,
    .name = "Test Space"
  };
  Rectangle rect = get_space_rectangle(&space);
  printf("Corners of the rectangle:\n");
  for (int i = 0; i < 4; i++) {
    printf("Corner %d: (%.2f, %.2f)\n", i, rect.corner[i].x, rect.corner[i].y);
  }
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_get_endpoint);
	RUN_TEST(test_get_space_rectangle);
	return UNITY_END();
}
