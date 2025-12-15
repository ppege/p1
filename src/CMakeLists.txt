add_executable(main main.c)
target_link_libraries(main
                      PlateDB
                      data
                      display
                      image
                      lot
                      lotReader
                      nav
                      validate)
