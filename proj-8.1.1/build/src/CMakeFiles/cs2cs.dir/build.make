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
include src/CMakeFiles/cs2cs.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/cs2cs.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/cs2cs.dir/flags.make

src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o: src/CMakeFiles/cs2cs.dir/flags.make
src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o: ../src/apps/cs2cs.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/cs2cs.cpp

src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/cs2cs.cpp > CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.i

src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/cs2cs.cpp -o CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.s

src/CMakeFiles/cs2cs.dir/apps/emess.cpp.o: src/CMakeFiles/cs2cs.dir/flags.make
src/CMakeFiles/cs2cs.dir/apps/emess.cpp.o: ../src/apps/emess.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/CMakeFiles/cs2cs.dir/apps/emess.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cs2cs.dir/apps/emess.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/emess.cpp

src/CMakeFiles/cs2cs.dir/apps/emess.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cs2cs.dir/apps/emess.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/emess.cpp > CMakeFiles/cs2cs.dir/apps/emess.cpp.i

src/CMakeFiles/cs2cs.dir/apps/emess.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cs2cs.dir/apps/emess.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/emess.cpp -o CMakeFiles/cs2cs.dir/apps/emess.cpp.s

src/CMakeFiles/cs2cs.dir/apps/utils.cpp.o: src/CMakeFiles/cs2cs.dir/flags.make
src/CMakeFiles/cs2cs.dir/apps/utils.cpp.o: ../src/apps/utils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/CMakeFiles/cs2cs.dir/apps/utils.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cs2cs.dir/apps/utils.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/utils.cpp

src/CMakeFiles/cs2cs.dir/apps/utils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cs2cs.dir/apps/utils.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/utils.cpp > CMakeFiles/cs2cs.dir/apps/utils.cpp.i

src/CMakeFiles/cs2cs.dir/apps/utils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cs2cs.dir/apps/utils.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/utils.cpp -o CMakeFiles/cs2cs.dir/apps/utils.cpp.s

# Object files for target cs2cs
cs2cs_OBJECTS = \
"CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o" \
"CMakeFiles/cs2cs.dir/apps/emess.cpp.o" \
"CMakeFiles/cs2cs.dir/apps/utils.cpp.o"

# External object files for target cs2cs
cs2cs_EXTERNAL_OBJECTS =

bin/cs2cs: src/CMakeFiles/cs2cs.dir/apps/cs2cs.cpp.o
bin/cs2cs: src/CMakeFiles/cs2cs.dir/apps/emess.cpp.o
bin/cs2cs: src/CMakeFiles/cs2cs.dir/apps/utils.cpp.o
bin/cs2cs: src/CMakeFiles/cs2cs.dir/build.make
bin/cs2cs: lib/libproj.so.22.1.1
bin/cs2cs: src/CMakeFiles/cs2cs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable ../bin/cs2cs"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cs2cs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/cs2cs.dir/build: bin/cs2cs

.PHONY : src/CMakeFiles/cs2cs.dir/build

src/CMakeFiles/cs2cs.dir/clean:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && $(CMAKE_COMMAND) -P CMakeFiles/cs2cs.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/cs2cs.dir/clean

src/CMakeFiles/cs2cs.dir/depend:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/sdb2/Progetti/Extruder/proj-8.1.1 /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src/CMakeFiles/cs2cs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/cs2cs.dir/depend
