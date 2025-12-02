#include "unity.h"
#include "data.h"
#include "calculations.h"
#include <stdio.h>
#include <string.h>

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
  Space test_space = {
    .type = Standard,
    .location = {10.0, 5.0, 0},
    .rotation = 45.0,
    .name = "Test Space"
  };

  Rectangle rect = get_space_rectangle(&test_space);
  Rectangle expected_rectangle = {
    .corner = {
      {10.00, 5.00},
      {11.77, 6.77},
      {8.23, 10.30},
      {6.46, 8.54}
    }
  };

  double delta = 1e-2;
  for (int i = 0; i < 4; i++) {
    char message_x[50];
    char message_y[50];
    sprintf(message_x, "corner %d x coordinate incorrect", i);
    sprintf(message_y, "corner %d y coordinate incorrect", i);
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected_rectangle.corner[i].x, rect.corner[i].x, message_x);
    TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected_rectangle.corner[i].y, rect.corner[i].y, message_y);
  }
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_get_endpoint);
	RUN_TEST(test_get_space_rectangle);
	return UNITY_END();
}
