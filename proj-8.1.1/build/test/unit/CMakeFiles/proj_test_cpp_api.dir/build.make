# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /mnt/sdb2/Progetti/Extruder/proj-8.1.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build

# Include any dependencies generated for this target.
include test/unit/CMakeFiles/proj_test_cpp_api.dir/depend.make

# Include the progress variables for this target.
include test/unit/CMakeFiles/proj_test_cpp_api.dir/progress.make

# Include the compile flags for this target's objects.
include test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make

test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.o: ../test/unit/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/main.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/main.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/main.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/main.cpp > CMakeFiles/proj_test_cpp_api.dir/main.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/main.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/main.cpp -o CMakeFiles/proj_test_cpp_api.dir/main.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o: ../test/unit/test_util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_util.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_util.cpp > CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_util.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o: ../test/unit/test_common.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_common.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_common.cpp > CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_common.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o: ../test/unit/test_crs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_crs.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_crs.cpp > CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_crs.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o: ../test/unit/test_metadata.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_metadata.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_metadata.cpp > CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_metadata.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o: ../test/unit/test_io.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_io.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_io.cpp > CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_io.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o: ../test/unit/test_operation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operation.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operation.cpp > CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operation.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o: ../test/unit/test_operationfactory.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operationfactory.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operationfactory.cpp > CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_operationfactory.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o: ../test/unit/test_datum.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_datum.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_datum.cpp > CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_datum.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o: ../test/unit/test_factory.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_factory.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_factory.cpp > CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_factory.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o: ../test/unit/test_c_api.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_c_api.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_c_api.cpp > CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_c_api.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.s

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o: test/unit/CMakeFiles/proj_test_cpp_api.dir/flags.make
test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o: ../test/unit/test_grids.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_grids.cpp

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_grids.cpp > CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.i

test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit/test_grids.cpp -o CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.s

# Object files for target proj_test_cpp_api
proj_test_cpp_api_OBJECTS = \
"CMakeFiles/proj_test_cpp_api.dir/main.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o" \
"CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o"

# External object files for target proj_test_cpp_api
proj_test_cpp_api_EXTERNAL_OBJECTS =

bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/main.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_util.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_common.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_crs.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_metadata.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_io.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operation.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_operationfactory.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_datum.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_factory.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_c_api.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/test_grids.cpp.o
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/build.make
bin/proj_test_cpp_api: lib/libgtest.so
bin/proj_test_cpp_api: lib/libproj.so.22.1.1
bin/proj_test_cpp_api: /usr/lib/x86_64-linux-gnu/libsqlite3.so
bin/proj_test_cpp_api: test/unit/CMakeFiles/proj_test_cpp_api.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Linking CXX executable ../../bin/proj_test_cpp_api"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/proj_test_cpp_api.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/unit/CMakeFiles/proj_test_cpp_api.dir/build: bin/proj_test_cpp_api

.PHONY : test/unit/CMakeFiles/proj_test_cpp_api.dir/build

test/unit/CMakeFiles/proj_test_cpp_api.dir/clean:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit && $(CMAKE_COMMAND) -P CMakeFiles/proj_test_cpp_api.dir/cmake_clean.cmake
.PHONY : test/unit/CMakeFiles/proj_test_cpp_api.dir/clean

test/unit/CMakeFiles/proj_test_cpp_api.dir/depend:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/sdb2/Progetti/Extruder/proj-8.1.1 /mnt/sdb2/Progetti/Extruder/proj-8.1.1/test/unit /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/test/unit/CMakeFiles/proj_test_cpp_api.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/unit/CMakeFiles/proj_test_cpp_api.dir/depend

