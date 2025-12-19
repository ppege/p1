# Parking lot manager

The large amount of densely packed cars found in urban environments results in greater emissions due to time wastage brought on by suboptimal parking conditions. In order to help reduce these issues, this report presents a software tool which assigns incoming drivers a specific spot to park at in a given parking garage and visualizes navigational instructions. This software was implemented in C, and made to be interfaced with through the command line. Additionally, a graphical subcomponent for the navigation was added. The program, at its core, functions by tracking the state of a parking lot using data structures. The utilization of a custom parser allows for deserializing the contents of a file into a C data structure representing a parking lot. Leveraging vector mathematics to reconstruct a parking lot enables the data structure to maintain simplicity while maximizing flexibility and functionality. Following all of the earlier specifications resulted in a program that satisfied the requirements derived from the problem statement.

## Building

First, this project has only been tested on Linux and MacOS. Windows is untested.

First you have to setup Unity test as a git submodule.

```bash
git submodule init
git submodule update
```

Then you can create a build folder and run CMake to generate the build files.

```bash
cmake -S . -B build
```

Make sure you first build the program with the build command.

```bash
cmake --build build 
```

After that you can run the program from the build folder.

```bash
./build/src/main
```

## tests

In this project we do unit testing with the Unity test framework.
All of the tests is placed in the `./test` folder. Every file in the folder should test a library.

To run the tests, use CTest.

```bash
ctest --test-dir build
```

Then the tests should have been run succesfully.
