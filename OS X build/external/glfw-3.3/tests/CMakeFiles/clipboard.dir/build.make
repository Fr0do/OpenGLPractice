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

# Include any dependencies generated for this target.
include external/glfw-3.3/tests/CMakeFiles/clipboard.dir/depend.make

# Include the progress variables for this target.
include external/glfw-3.3/tests/CMakeFiles/clipboard.dir/progress.make

# Include the compile flags for this target's objects.
include external/glfw-3.3/tests/CMakeFiles/clipboard.dir/flags.make

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.o: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/flags.make
external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.o: ../external/glfw-3.3/tests/clipboard.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.o"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/clipboard.dir/clipboard.c.o   -c "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/tests/clipboard.c"

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/clipboard.c.i"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/tests/clipboard.c" > CMakeFiles/clipboard.dir/clipboard.c.i

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/clipboard.c.s"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/tests/clipboard.c" -o CMakeFiles/clipboard.dir/clipboard.c.s

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.o: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/flags.make
external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.o: ../external/glfw-3.3/deps/getopt.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.o"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/clipboard.dir/__/deps/getopt.c.o   -c "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/getopt.c"

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/__/deps/getopt.c.i"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/getopt.c" > CMakeFiles/clipboard.dir/__/deps/getopt.c.i

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/__/deps/getopt.c.s"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/getopt.c" -o CMakeFiles/clipboard.dir/__/deps/getopt.c.s

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/flags.make
external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o: ../external/glfw-3.3/deps/glad_gl.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building C object external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o   -c "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/glad_gl.c"

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/clipboard.dir/__/deps/glad_gl.c.i"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/glad_gl.c" > CMakeFiles/clipboard.dir/__/deps/glad_gl.c.i

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/clipboard.dir/__/deps/glad_gl.c.s"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && /Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/deps/glad_gl.c" -o CMakeFiles/clipboard.dir/__/deps/glad_gl.c.s

# Object files for target clipboard
clipboard_OBJECTS = \
"CMakeFiles/clipboard.dir/clipboard.c.o" \
"CMakeFiles/clipboard.dir/__/deps/getopt.c.o" \
"CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o"

# External object files for target clipboard
clipboard_EXTERNAL_OBJECTS =

external/glfw-3.3/tests/clipboard: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clipboard.c.o
external/glfw-3.3/tests/clipboard: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/getopt.c.o
external/glfw-3.3/tests/clipboard: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/__/deps/glad_gl.c.o
external/glfw-3.3/tests/clipboard: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/build.make
external/glfw-3.3/tests/clipboard: external/glfw-3.3/src/libglfw3.a
external/glfw-3.3/tests/clipboard: external/glfw-3.3/tests/CMakeFiles/clipboard.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable clipboard"
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/clipboard.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
external/glfw-3.3/tests/CMakeFiles/clipboard.dir/build: external/glfw-3.3/tests/clipboard

.PHONY : external/glfw-3.3/tests/CMakeFiles/clipboard.dir/build

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clean:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" && $(CMAKE_COMMAND) -P CMakeFiles/clipboard.dir/cmake_clean.cmake
.PHONY : external/glfw-3.3/tests/CMakeFiles/clipboard.dir/clean

external/glfw-3.3/tests/CMakeFiles/clipboard.dir/depend:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/maximkurkin/Downloads/5 семестр/openglpractice" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/external/glfw-3.3/tests" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/external/glfw-3.3/tests/CMakeFiles/clipboard.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : external/glfw-3.3/tests/CMakeFiles/clipboard.dir/depend

