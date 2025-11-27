#include "unity.h"
#include "data.h"

void setUp() {
}

void tearDown() {
}

void test_get_endpoint(void) {
  Path path = {
    .vector = {3.0, 4.0},
    .start_point = {1.0, 2.0, 0}
  };
  Location endpoint = get_endpoint(&path);
  TEST_ASSERT_EQUAL_FLOAT(4.0, endpoint.x);
  TEST_ASSERT_EQUAL_FLOAT(6.0, endpoint.y);
  TEST_ASSERT_EQUAL_INT(0, endpoint.level);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_get_endpoint);
	return UNITY_END();
}
