# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build"

# Include any dependencies generated for this target.
include core/shell/CMakeFiles/shell-lib.dir/depend.make

# Include the progress variables for this target.
include core/shell/CMakeFiles/shell-lib.dir/progress.make

# Include the compile flags for this target's objects.
include core/shell/CMakeFiles/shell-lib.dir/flags.make

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o: core/shell/CMakeFiles/shell-lib.dir/flags.make
core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o: core/shell/modules.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o"
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/shell-lib.dir/modules.cpp.o -c "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell/modules.cpp"

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/shell-lib.dir/modules.cpp.i"
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell/modules.cpp" > CMakeFiles/shell-lib.dir/modules.cpp.i

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/shell-lib.dir/modules.cpp.s"
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell/modules.cpp" -o CMakeFiles/shell-lib.dir/modules.cpp.s

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.requires:

.PHONY : core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.requires

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.provides: core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.requires
	$(MAKE) -f core/shell/CMakeFiles/shell-lib.dir/build.make core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.provides.build
.PHONY : core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.provides

core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.provides.build: core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o


# Object files for target shell-lib
shell__lib_OBJECTS = \
"CMakeFiles/shell-lib.dir/modules.cpp.o"

# External object files for target shell-lib
shell__lib_EXTERNAL_OBJECTS =

core/shell/libshell-lib.a: core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o
core/shell/libshell-lib.a: core/shell/CMakeFiles/shell-lib.dir/build.make
core/shell/libshell-lib.a: core/shell/CMakeFiles/shell-lib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libshell-lib.a"
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && $(CMAKE_COMMAND) -P CMakeFiles/shell-lib.dir/cmake_clean_target.cmake
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/shell-lib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
core/shell/CMakeFiles/shell-lib.dir/build: core/shell/libshell-lib.a

.PHONY : core/shell/CMakeFiles/shell-lib.dir/build

core/shell/CMakeFiles/shell-lib.dir/requires: core/shell/CMakeFiles/shell-lib.dir/modules.cpp.o.requires

.PHONY : core/shell/CMakeFiles/shell-lib.dir/requires

core/shell/CMakeFiles/shell-lib.dir/clean:
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" && $(CMAKE_COMMAND) -P CMakeFiles/shell-lib.dir/cmake_clean.cmake
.PHONY : core/shell/CMakeFiles/shell-lib.dir/clean

core/shell/CMakeFiles/shell-lib.dir/depend:
	cd "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants" "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/shell" "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build" "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell" "/Users/lorenzocioni/Documents/Scuola/Ingegneria/Laurea Magistrale/Tesi/source/illuminants/build/core/shell/CMakeFiles/shell-lib.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : core/shell/CMakeFiles/shell-lib.dir/depend

