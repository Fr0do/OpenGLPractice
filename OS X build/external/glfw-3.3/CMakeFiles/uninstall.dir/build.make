# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.15.5/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.15.5/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/maximkurkin/Downloads/5 семестр/openglpractice"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build"

# Utility rule file for uninstall.

# Include the progress variables for this target.
include external/glfw-3.3/CMakeFiles/uninstall.dir/progress.make

external/glfw-3.3/CMakeFiles/uninstall:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3" && /usr/local/Cellar/cmake/3.15.5/bin/cmake -P /Users/maximkurkin/Downloads/5\ семестр/openglpractice/build/external/glfw-3.3/cmake_uninstall.cmake

uninstall: external/glfw-3.3/CMakeFiles/uninstall
uninstall: external/glfw-3.3/CMakeFiles/uninstall.dir/build.make

.PHONY : uninstall

# Rule to build all files generated by this target.
external/glfw-3.3/CMakeFiles/uninstall.dir/build: uninstall

.PHONY : external/glfw-3.3/CMakeFiles/uninstall.dir/build

external/glfw-3.3/CMakeFiles/uninstall.dir/clean:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3" && $(CMAKE_COMMAND) -P CMakeFiles/uninstall.dir/cmake_clean.cmake
.PHONY : external/glfw-3.3/CMakeFiles/uninstall.dir/clean

external/glfw-3.3/CMakeFiles/uninstall.dir/depend:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/maximkurkin/Downloads/5 семестр/openglpractice" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/CMakeFiles/uninstall.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : external/glfw-3.3/CMakeFiles/uninstall.dir/depend

