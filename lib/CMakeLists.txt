add_library(data data.c)
target_include_directories(data PUBLIC .)

add_library(lot lot.c)
target_include_directories(lot PUBLIC .)

add_library(display display.c)
target_include_directories(display PUBLIC .)

add_library(overview overview.c)
target_include_directories(overview PUBLIC .)

add_library(PlateDB PlateDB.c)
target_include_directories(PlateDB PUBLIC .)

add_library(Unity STATIC external/Unity/src/unity.c)
target_include_directories(Unity PUBLIC external/Unity/src)

