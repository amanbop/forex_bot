# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux

# Include any dependencies generated for this target.
include CMakeFiles/ForexConnect_Sample_Linux.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ForexConnect_Sample_Linux.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ForexConnect_Sample_Linux.dir/flags.make

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o: CMakeFiles/ForexConnect_Sample_Linux.dir/flags.make
CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o: source/stdafx.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/stdafx.cpp

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/stdafx.cpp > CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.i

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/stdafx.cpp -o CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.s

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.requires:
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.requires

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.provides: CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.requires
	$(MAKE) -f CMakeFiles/ForexConnect_Sample_Linux.dir/build.make CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.provides.build
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.provides

CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.provides.build: CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.provides.build

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o: CMakeFiles/ForexConnect_Sample_Linux.dir/flags.make
CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o: source/sample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/sample.cpp

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/sample.cpp > CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.i

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/source/sample.cpp -o CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.s

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.requires:
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.requires

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.provides: CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.requires
	$(MAKE) -f CMakeFiles/ForexConnect_Sample_Linux.dir/build.make CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.provides.build
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.provides

CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.provides.build: CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.provides.build

# Object files for target ForexConnect_Sample_Linux
ForexConnect_Sample_Linux_OBJECTS = \
"CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o" \
"CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o"

# External object files for target ForexConnect_Sample_Linux
ForexConnect_Sample_Linux_EXTERNAL_OBJECTS =

ForexConnect_Sample_Linux: CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o
ForexConnect_Sample_Linux: CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o
ForexConnect_Sample_Linux: CMakeFiles/ForexConnect_Sample_Linux.dir/build.make
ForexConnect_Sample_Linux: CMakeFiles/ForexConnect_Sample_Linux.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ForexConnect_Sample_Linux"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ForexConnect_Sample_Linux.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ForexConnect_Sample_Linux.dir/build: ForexConnect_Sample_Linux
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/build

CMakeFiles/ForexConnect_Sample_Linux.dir/requires: CMakeFiles/ForexConnect_Sample_Linux.dir/source/stdafx.cpp.o.requires
CMakeFiles/ForexConnect_Sample_Linux.dir/requires: CMakeFiles/ForexConnect_Sample_Linux.dir/source/sample.cpp.o.requires
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/requires

CMakeFiles/ForexConnect_Sample_Linux.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ForexConnect_Sample_Linux.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/clean

CMakeFiles/ForexConnect_Sample_Linux.dir/depend:
	cd /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux /home/adminuser/Documents/forex_development/ForexConnect_Sample_Linux/CMakeFiles/ForexConnect_Sample_Linux.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ForexConnect_Sample_Linux.dir/depend

