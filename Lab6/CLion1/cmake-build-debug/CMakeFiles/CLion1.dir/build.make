# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /home/mateusz/Documents/clion-2018.1.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/mateusz/Documents/clion-2018.1.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/CLion1.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CLion1.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CLion1.dir/flags.make

CMakeFiles/CLion1.dir/client.c.o: CMakeFiles/CLion1.dir/flags.make
CMakeFiles/CLion1.dir/client.c.o: ../client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CLion1.dir/client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CLion1.dir/client.c.o   -c /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/client.c

CMakeFiles/CLion1.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CLion1.dir/client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/client.c > CMakeFiles/CLion1.dir/client.c.i

CMakeFiles/CLion1.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CLion1.dir/client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/client.c -o CMakeFiles/CLion1.dir/client.c.s

CMakeFiles/CLion1.dir/client.c.o.requires:

.PHONY : CMakeFiles/CLion1.dir/client.c.o.requires

CMakeFiles/CLion1.dir/client.c.o.provides: CMakeFiles/CLion1.dir/client.c.o.requires
	$(MAKE) -f CMakeFiles/CLion1.dir/build.make CMakeFiles/CLion1.dir/client.c.o.provides.build
.PHONY : CMakeFiles/CLion1.dir/client.c.o.provides

CMakeFiles/CLion1.dir/client.c.o.provides.build: CMakeFiles/CLion1.dir/client.c.o


CMakeFiles/CLion1.dir/server.c.o: CMakeFiles/CLion1.dir/flags.make
CMakeFiles/CLion1.dir/server.c.o: ../server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/CLion1.dir/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CLion1.dir/server.c.o   -c /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/server.c

CMakeFiles/CLion1.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CLion1.dir/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/server.c > CMakeFiles/CLion1.dir/server.c.i

CMakeFiles/CLion1.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CLion1.dir/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/server.c -o CMakeFiles/CLion1.dir/server.c.s

CMakeFiles/CLion1.dir/server.c.o.requires:

.PHONY : CMakeFiles/CLion1.dir/server.c.o.requires

CMakeFiles/CLion1.dir/server.c.o.provides: CMakeFiles/CLion1.dir/server.c.o.requires
	$(MAKE) -f CMakeFiles/CLion1.dir/build.make CMakeFiles/CLion1.dir/server.c.o.provides.build
.PHONY : CMakeFiles/CLion1.dir/server.c.o.provides

CMakeFiles/CLion1.dir/server.c.o.provides.build: CMakeFiles/CLion1.dir/server.c.o


# Object files for target CLion1
CLion1_OBJECTS = \
"CMakeFiles/CLion1.dir/client.c.o" \
"CMakeFiles/CLion1.dir/server.c.o"

# External object files for target CLion1
CLion1_EXTERNAL_OBJECTS =

CLion1: CMakeFiles/CLion1.dir/client.c.o
CLion1: CMakeFiles/CLion1.dir/server.c.o
CLion1: CMakeFiles/CLion1.dir/build.make
CLion1: CMakeFiles/CLion1.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable CLion1"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CLion1.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CLion1.dir/build: CLion1

.PHONY : CMakeFiles/CLion1.dir/build

CMakeFiles/CLion1.dir/requires: CMakeFiles/CLion1.dir/client.c.o.requires
CMakeFiles/CLion1.dir/requires: CMakeFiles/CLion1.dir/server.c.o.requires

.PHONY : CMakeFiles/CLion1.dir/requires

CMakeFiles/CLion1.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CLion1.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CLion1.dir/clean

CMakeFiles/CLion1.dir/depend:
	cd /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1 /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1 /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug /home/mateusz/Documents/SysOpyStudyCourse2018/Lab6/CLion1/cmake-build-debug/CMakeFiles/CLion1.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CLion1.dir/depend

