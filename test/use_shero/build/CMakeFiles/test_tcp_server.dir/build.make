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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hadoop/shero/test/use_shero

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hadoop/shero/test/use_shero/build

# Include any dependencies generated for this target.
include CMakeFiles/test_tcp_server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_tcp_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_tcp_server.dir/flags.make

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o: CMakeFiles/test_tcp_server.dir/flags.make
CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o: ../test_tcp_server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/hadoop/shero/test/use_shero/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o -c /home/hadoop/shero/test/use_shero/test_tcp_server.cc

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/hadoop/shero/test/use_shero/test_tcp_server.cc > CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.i

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/hadoop/shero/test/use_shero/test_tcp_server.cc -o CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.s

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.requires:

.PHONY : CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.requires

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.provides: CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.requires
	$(MAKE) -f CMakeFiles/test_tcp_server.dir/build.make CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.provides.build
.PHONY : CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.provides

CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.provides.build: CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o


# Object files for target test_tcp_server
test_tcp_server_OBJECTS = \
"CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o"

# External object files for target test_tcp_server
test_tcp_server_EXTERNAL_OBJECTS =

test_tcp_server: CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o
test_tcp_server: CMakeFiles/test_tcp_server.dir/build.make
test_tcp_server: /usr/local/lib/libshero.a
test_tcp_server: /usr/local/lib/libyaml-cpp.a
test_tcp_server: /usr/lib/libtinyxml.a
test_tcp_server: /usr/lib/x86_64-linux-gnu/libssl.so
test_tcp_server: /usr/lib/x86_64-linux-gnu/libcrypto.so
test_tcp_server: CMakeFiles/test_tcp_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/hadoop/shero/test/use_shero/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_tcp_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_tcp_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_tcp_server.dir/build: test_tcp_server

.PHONY : CMakeFiles/test_tcp_server.dir/build

CMakeFiles/test_tcp_server.dir/requires: CMakeFiles/test_tcp_server.dir/test_tcp_server.cc.o.requires

.PHONY : CMakeFiles/test_tcp_server.dir/requires

CMakeFiles/test_tcp_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_tcp_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_tcp_server.dir/clean

CMakeFiles/test_tcp_server.dir/depend:
	cd /home/hadoop/shero/test/use_shero/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hadoop/shero/test/use_shero /home/hadoop/shero/test/use_shero /home/hadoop/shero/test/use_shero/build /home/hadoop/shero/test/use_shero/build /home/hadoop/shero/test/use_shero/build/CMakeFiles/test_tcp_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_tcp_server.dir/depend

