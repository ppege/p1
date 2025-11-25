add_library(data data.c)
target_include_directories(data PUBLIC .)

add_library(PlateDB PlateDB.c)
target_include_directories(PlateDB PUBLIC .)

add_library(Unity STATIC external/Unity/src/unity.c)
target_include_directories(Unity PUBLIC external/Unity/src)
