#include "unity.h"

void setUp() {}

void tearDown() {}

void test_lines(void) {
  int lines = 3;
  TEST_ASSERT_EQUAL_INT_MESSAGE(3, lines,
                                "Checking amount of lines in test file");
}

void test_num(void) {
  int num = 0;
  TEST_ASSERT_EQUAL_INT(0, num);
}

void test_double_67(void) {
  float num = 6.7;
  TEST_ASSERT_FLOAT_WITHIN(0.0001, 6.7, num);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lines);
  RUN_TEST(test_num);
  RUN_TEST(test_double_67);
  return UNITY_END();
}
