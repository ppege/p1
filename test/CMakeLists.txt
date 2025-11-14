add_executable(test_1 test.c)
target_link_libraries(test_1 data Unity)

add_test(NAME Test_1 COMMAND test_1)
