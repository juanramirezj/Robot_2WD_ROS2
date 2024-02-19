# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/administrator/robot2wd/final/basic_test/motor4_driver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/administrator/robot2wd/final/basic_test/motor4_driver/build

# Utility rule file for motor4_driver_quadrature_encoder_pio_h.

# Include any custom commands dependencies for this target.
include CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/progress.make

CMakeFiles/motor4_driver_quadrature_encoder_pio_h: quadrature_encoder.pio.h

quadrature_encoder.pio.h: ../quadrature_encoder.pio
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/administrator/robot2wd/final/basic_test/motor4_driver/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating quadrature_encoder.pio.h"
	pioasm/pioasm -o c-sdk /home/administrator/robot2wd/final/basic_test/motor4_driver/quadrature_encoder.pio /home/administrator/robot2wd/final/basic_test/motor4_driver/build/quadrature_encoder.pio.h

motor4_driver_quadrature_encoder_pio_h: CMakeFiles/motor4_driver_quadrature_encoder_pio_h
motor4_driver_quadrature_encoder_pio_h: quadrature_encoder.pio.h
motor4_driver_quadrature_encoder_pio_h: CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/build.make
.PHONY : motor4_driver_quadrature_encoder_pio_h

# Rule to build all files generated by this target.
CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/build: motor4_driver_quadrature_encoder_pio_h
.PHONY : CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/build

CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/cmake_clean.cmake
.PHONY : CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/clean

CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/depend:
	cd /home/administrator/robot2wd/final/basic_test/motor4_driver/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/administrator/robot2wd/final/basic_test/motor4_driver /home/administrator/robot2wd/final/basic_test/motor4_driver /home/administrator/robot2wd/final/basic_test/motor4_driver/build /home/administrator/robot2wd/final/basic_test/motor4_driver/build /home/administrator/robot2wd/final/basic_test/motor4_driver/build/CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/motor4_driver_quadrature_encoder_pio_h.dir/depend

