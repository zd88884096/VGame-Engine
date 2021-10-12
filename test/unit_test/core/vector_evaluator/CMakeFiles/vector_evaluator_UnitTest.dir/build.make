# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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
CMAKE_COMMAND = /usr/bin/cmake.exe

# The command to remove a file.
RM = /usr/bin/cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test"

# Include any dependencies generated for this target.
include unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/depend.make

# Include the progress variables for this target.
include unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/progress.make

# Include the compile flags for this target's objects.
include unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/flags.make

unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o: unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/flags.make
unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o: unit_test/core/vector_evaluator/unitTest.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o"
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" && /usr/bin/c++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o -c "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator/unitTest.cpp"

unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.i"
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" && /usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator/unitTest.cpp" > CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.i

unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.s"
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" && /usr/bin/c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator/unitTest.cpp" -o CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.s

# Object files for target vector_evaluator_UnitTest
vector_evaluator_UnitTest_OBJECTS = \
"CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o"

# External object files for target vector_evaluator_UnitTest
vector_evaluator_UnitTest_EXTERNAL_OBJECTS =

unit_test/core/vector_evaluator/vector_evaluator_UnitTest.exe: unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/unitTest.cpp.o
unit_test/core/vector_evaluator/vector_evaluator_UnitTest.exe: unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/build.make
unit_test/core/vector_evaluator/vector_evaluator_UnitTest.exe: unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable vector_evaluator_UnitTest.exe"
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vector_evaluator_UnitTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/build: unit_test/core/vector_evaluator/vector_evaluator_UnitTest.exe

.PHONY : unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/build

unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/clean:
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" && $(CMAKE_COMMAND) -P CMakeFiles/vector_evaluator_UnitTest.dir/cmake_clean.cmake
.PHONY : unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/clean

unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/depend:
	cd "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test" "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test" "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator" "/cygdrive/C/users/zd888/OneDrive/Desktop/Projects/C++ Math Lib/vmathlibrary/test/unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : unit_test/core/vector_evaluator/CMakeFiles/vector_evaluator_UnitTest.dir/depend

