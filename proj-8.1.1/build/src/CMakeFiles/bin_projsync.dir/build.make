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
include src/CMakeFiles/bin_projsync.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/bin_projsync.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/bin_projsync.dir/flags.make

src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o: src/CMakeFiles/bin_projsync.dir/flags.make
src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o: ../src/apps/projsync.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o -c /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/projsync.cpp

src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/bin_projsync.dir/apps/projsync.cpp.i"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/projsync.cpp > CMakeFiles/bin_projsync.dir/apps/projsync.cpp.i

src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/bin_projsync.dir/apps/projsync.cpp.s"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src/apps/projsync.cpp -o CMakeFiles/bin_projsync.dir/apps/projsync.cpp.s

# Object files for target bin_projsync
bin_projsync_OBJECTS = \
"CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o"

# External object files for target bin_projsync
bin_projsync_EXTERNAL_OBJECTS =

bin/projsync: src/CMakeFiles/bin_projsync.dir/apps/projsync.cpp.o
bin/projsync: src/CMakeFiles/bin_projsync.dir/build.make
bin/projsync: lib/libproj.so.22.1.1
bin/projsync: src/CMakeFiles/bin_projsync.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/projsync"
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bin_projsync.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/bin_projsync.dir/build: bin/projsync

.PHONY : src/CMakeFiles/bin_projsync.dir/build

src/CMakeFiles/bin_projsync.dir/clean:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src && $(CMAKE_COMMAND) -P CMakeFiles/bin_projsync.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/bin_projsync.dir/clean

src/CMakeFiles/bin_projsync.dir/depend:
	cd /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/sdb2/Progetti/Extruder/proj-8.1.1 /mnt/sdb2/Progetti/Extruder/proj-8.1.1/src /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src /mnt/sdb2/Progetti/Extruder/proj-8.1.1/build/src/CMakeFiles/bin_projsync.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/bin_projsync.dir/depend

