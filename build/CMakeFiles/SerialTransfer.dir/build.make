# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /root/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/test/build

# Include any dependencies generated for this target.
include CMakeFiles/SerialTransfer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/SerialTransfer.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/SerialTransfer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/SerialTransfer.dir/flags.make

CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o: CMakeFiles/SerialTransfer.dir/flags.make
CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o: ../linux_serial_receive.c
CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o: CMakeFiles/SerialTransfer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o"
	/usr/bin/arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o -MF CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o.d -o CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o -c /root/test/linux_serial_receive.c

CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.i"
	/usr/bin/arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/test/linux_serial_receive.c > CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.i

CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.s"
	/usr/bin/arm-linux-gnueabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/test/linux_serial_receive.c -o CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.s

# Object files for target SerialTransfer
SerialTransfer_OBJECTS = \
"CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o"

# External object files for target SerialTransfer
SerialTransfer_EXTERNAL_OBJECTS =

SerialTransfer: CMakeFiles/SerialTransfer.dir/linux_serial_receive.c.o
SerialTransfer: CMakeFiles/SerialTransfer.dir/build.make
SerialTransfer: CMakeFiles/SerialTransfer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable SerialTransfer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/SerialTransfer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/SerialTransfer.dir/build: SerialTransfer
.PHONY : CMakeFiles/SerialTransfer.dir/build

CMakeFiles/SerialTransfer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/SerialTransfer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/SerialTransfer.dir/clean

CMakeFiles/SerialTransfer.dir/depend:
	cd /root/test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/test /root/test /root/test/build /root/test/build /root/test/build/CMakeFiles/SerialTransfer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/SerialTransfer.dir/depend

