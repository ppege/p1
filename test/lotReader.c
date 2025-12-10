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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lot_from_file);
  return UNITY_END();
}
