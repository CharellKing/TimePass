# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ck/TimePass/example

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ck/TimePass/example/build

# Include any dependencies generated for this target.
include daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/depend.make

# Include the progress variables for this target.
include daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/progress.make

# Include the compile flags for this target's objects.
include daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/flags.make

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/flags.make
daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o: ../daytimetcpsrv/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/ck/TimePass/example/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o"
	cd /home/ck/TimePass/example/build/daytimetcpsrv && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/daytimetcpsrv.dir/main.cpp.o -c /home/ck/TimePass/example/daytimetcpsrv/main.cpp

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/daytimetcpsrv.dir/main.cpp.i"
	cd /home/ck/TimePass/example/build/daytimetcpsrv && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/ck/TimePass/example/daytimetcpsrv/main.cpp > CMakeFiles/daytimetcpsrv.dir/main.cpp.i

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/daytimetcpsrv.dir/main.cpp.s"
	cd /home/ck/TimePass/example/build/daytimetcpsrv && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/ck/TimePass/example/daytimetcpsrv/main.cpp -o CMakeFiles/daytimetcpsrv.dir/main.cpp.s

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.requires:
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.requires

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.provides: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.requires
	$(MAKE) -f daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/build.make daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.provides.build
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.provides

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.provides.build: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o

# Object files for target daytimetcpsrv
daytimetcpsrv_OBJECTS = \
"CMakeFiles/daytimetcpsrv.dir/main.cpp.o"

# External object files for target daytimetcpsrv
daytimetcpsrv_EXTERNAL_OBJECTS =

daytimetcpsrv/daytimetcpsrv: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o
daytimetcpsrv/daytimetcpsrv: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/build.make
daytimetcpsrv/daytimetcpsrv: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable daytimetcpsrv"
	cd /home/ck/TimePass/example/build/daytimetcpsrv && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/daytimetcpsrv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/build: daytimetcpsrv/daytimetcpsrv
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/build

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/requires: daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/main.cpp.o.requires
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/requires

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/clean:
	cd /home/ck/TimePass/example/build/daytimetcpsrv && $(CMAKE_COMMAND) -P CMakeFiles/daytimetcpsrv.dir/cmake_clean.cmake
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/clean

daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/depend:
	cd /home/ck/TimePass/example/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ck/TimePass/example /home/ck/TimePass/example/daytimetcpsrv /home/ck/TimePass/example/build /home/ck/TimePass/example/build/daytimetcpsrv /home/ck/TimePass/example/build/daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : daytimetcpsrv/CMakeFiles/daytimetcpsrv.dir/depend
