add_executable(test_1 test.c)
target_link_libraries(test_1 Unity)

add_executable(test_PlateDB PlateDB.c)
target_link_libraries(test_PlateDB data PlateDB Unity)

add_test(NAME Test_1 COMMAND test_1)
add_test(NAME test_PlateDB COMMAND test_PlateDB)
