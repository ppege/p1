#include "PlateDB.h"
#include "string.h"
#include "unity.h"

void setUp() {}

void tearDown() {}

char *FileName = "../../test/test.txt";

void test_read_file_lines(void) {
  int lines = GetFileLines(FileName);
  TEST_ASSERT_EQUAL_INT_MESSAGE(111, lines,
                                "Checking amount of lines in test file");
}

void test_read_line(void) {
  char *buffer = "AB12345 0";
  Car CarStruc = ReadLine(buffer);
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, strcmp(CarStruc.plate, "AB12345"),
                                "Checking plate");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, CarStruc.type, "Checking car type");
}

void test_read_file_to_struct(void) {
  int lines = 3;
  Car CarArr[3];
  ReadFile(CarArr, lines, FileName);
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, strcmp(CarArr[0].plate, "AB12345"),
                                "Checking first plate");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, strcmp(CarArr[1].plate, "EZ69420"),
                                "Checking second plate");
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, strcmp(CarArr[2].plate, "NO99999"),
                                "Checking third plate");
}

void test_get_plate_from_index(void) {
  int lines = 3;
  Car CarArr[3] = {{"AB12345", 0}, {"EZ69420", 1}, {"NO99999", 2}};
  TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                GetCarIndexFromPlate(CarArr, lines, "AB12345"),
                                "Checking first plate");
  TEST_ASSERT_EQUAL_INT_MESSAGE(1,
                                GetCarIndexFromPlate(CarArr, lines, "EZ69420"),
                                "Checking second plate");
  TEST_ASSERT_EQUAL_INT_MESSAGE(2,
                                GetCarIndexFromPlate(CarArr, lines, "NO99999"),
                                "Checking third plate");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_read_file_lines);
  RUN_TEST(test_read_line);
  RUN_TEST(test_read_file_to_struct);
  RUN_TEST(test_get_plate_from_index);
  return UNITY_END();
}
