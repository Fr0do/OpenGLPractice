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
include CMakeFiles/pbr.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/pbr.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/pbr.dir/flags.make

CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o: CMakeFiles/pbr.dir/flags.make
CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o: ../src/pbr/pbr.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o -c "/Users/maximkurkin/Downloads/5 семестр/openglpractice/src/pbr/pbr.cpp"

CMakeFiles/pbr.dir/src/pbr/pbr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/pbr.dir/src/pbr/pbr.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/maximkurkin/Downloads/5 семестр/openglpractice/src/pbr/pbr.cpp" > CMakeFiles/pbr.dir/src/pbr/pbr.cpp.i

CMakeFiles/pbr.dir/src/pbr/pbr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/pbr.dir/src/pbr/pbr.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/maximkurkin/Downloads/5 семестр/openglpractice/src/pbr/pbr.cpp" -o CMakeFiles/pbr.dir/src/pbr/pbr.cpp.s

# Object files for target pbr
pbr_OBJECTS = \
"CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o"

# External object files for target pbr
pbr_EXTERNAL_OBJECTS =

bin/pbr/pbr: CMakeFiles/pbr.dir/src/pbr/pbr.cpp.o
bin/pbr/pbr: CMakeFiles/pbr.dir/build.make
bin/pbr/pbr: external/glfw-3.3/src/libglfw3.a
bin/pbr/pbr: external/glfw-3.3/src/libglfw3.a
bin/pbr/pbr: libSTB_IMAGE.a
bin/pbr/pbr: libGLAD.a
bin/pbr/pbr: CMakeFiles/pbr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/pbr/pbr"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/pbr.dir/link.txt --verbose=$(VERBOSE)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "mklink /Users/maximkurkin/Downloads/5 семестр/openglpractice/src/pbr/pbr_frag.glsl -> /Users/maximkurkin/Downloads/5 семестр/openglpractice/build/bin/pbr/pbr_frag.glsl"
	/usr/local/Cellar/cmake/3.15.5/bin/cmake -E create_symlink /Users/maximkurkin/Downloads/5\ семестр/openglpractice/src/pbr/pbr_frag.glsl /Users/maximkurkin/Downloads/5\ семестр/openglpractice/build/bin/pbr/pbr_frag.glsl
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "mklink /Users/maximkurkin/Downloads/5 семестр/openglpractice/src/pbr/pbr_vert.glsl -> /Users/maximkurkin/Downloads/5 семестр/openglpractice/build/bin/pbr/pbr_vert.glsl"
	/usr/local/Cellar/cmake/3.15.5/bin/cmake -E create_symlink /Users/maximkurkin/Downloads/5\ семестр/openglpractice/src/pbr/pbr_vert.glsl /Users/maximkurkin/Downloads/5\ семестр/openglpractice/build/bin/pbr/pbr_vert.glsl

# Rule to build all files generated by this target.
CMakeFiles/pbr.dir/build: bin/pbr/pbr

.PHONY : CMakeFiles/pbr.dir/build

CMakeFiles/pbr.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/pbr.dir/cmake_clean.cmake
.PHONY : CMakeFiles/pbr.dir/clean

CMakeFiles/pbr.dir/depend:
	cd "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/maximkurkin/Downloads/5 семестр/openglpractice" "/Users/maximkurkin/Downloads/5 семестр/openglpractice" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build" "/Users/maximkurkin/Downloads/5 семестр/openglpractice/build/CMakeFiles/pbr.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/pbr.dir/depend

