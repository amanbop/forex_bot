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
CMAKE_SOURCE_DIR = /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample

# Include any dependencies generated for this target.
include CMakeFiles/WholeSample.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/WholeSample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WholeSample.dir/flags.make

CMakeFiles/WholeSample.dir/source/stdafx.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/stdafx.cpp.o: source/stdafx.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/stdafx.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/stdafx.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/stdafx.cpp

CMakeFiles/WholeSample.dir/source/stdafx.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/stdafx.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/stdafx.cpp > CMakeFiles/WholeSample.dir/source/stdafx.cpp.i

CMakeFiles/WholeSample.dir/source/stdafx.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/stdafx.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/stdafx.cpp -o CMakeFiles/WholeSample.dir/source/stdafx.cpp.s

CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.requires

CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.provides: CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.provides

CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/stdafx.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o: source/WholeSample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/WholeSample.cpp

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/WholeSample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/WholeSample.cpp > CMakeFiles/WholeSample.dir/source/WholeSample.cpp.i

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/WholeSample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/WholeSample.cpp -o CMakeFiles/WholeSample.dir/source/WholeSample.cpp.s

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.requires

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.provides: CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.provides

CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o: source/CreateOrderSample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/CreateOrderSample.cpp

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/CreateOrderSample.cpp > CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.i

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/CreateOrderSample.cpp -o CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.s

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.requires

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.provides: CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.provides

CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o: source/PermissionCheckerSample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/PermissionCheckerSample.cpp

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/PermissionCheckerSample.cpp > CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.i

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/PermissionCheckerSample.cpp -o CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.s

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.requires

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.provides: CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.provides

CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o: source/SimpleLog.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SimpleLog.cpp

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SimpleLog.cpp > CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.i

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SimpleLog.cpp -o CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.s

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.requires

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.provides: CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.provides

CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o: source/ResponseListener.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseListener.cpp

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseListener.cpp > CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.i

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseListener.cpp -o CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.s

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.requires

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.provides: CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.provides

CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o: source/ResponseQueue.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseQueue.cpp

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseQueue.cpp > CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.i

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/ResponseQueue.cpp -o CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.s

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.requires

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.provides: CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.provides

CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/EventListener.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/EventListener.cpp.o: source/EventListener.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/EventListener.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/EventListener.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/EventListener.cpp

CMakeFiles/WholeSample.dir/source/EventListener.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/EventListener.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/EventListener.cpp > CMakeFiles/WholeSample.dir/source/EventListener.cpp.i

CMakeFiles/WholeSample.dir/source/EventListener.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/EventListener.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/EventListener.cpp -o CMakeFiles/WholeSample.dir/source/EventListener.cpp.s

CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.requires

CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.provides: CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.provides

CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/EventListener.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o: source/SessionStatusListener.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_9)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SessionStatusListener.cpp

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SessionStatusListener.cpp > CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.i

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/SessionStatusListener.cpp -o CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.s

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.requires

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.provides: CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.provides

CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.provides.build

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o: CMakeFiles/WholeSample.dir/flags.make
CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o: source/TradingSettingsProviderSample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles $(CMAKE_PROGRESS_10)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o -c /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/TradingSettingsProviderSample.cpp

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/TradingSettingsProviderSample.cpp > CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.i

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/source/TradingSettingsProviderSample.cpp -o CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.s

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.requires:
.PHONY : CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.requires

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.provides: CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.requires
	$(MAKE) -f CMakeFiles/WholeSample.dir/build.make CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.provides.build
.PHONY : CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.provides

CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.provides.build: CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o
.PHONY : CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.provides.build

# Object files for target WholeSample
WholeSample_OBJECTS = \
"CMakeFiles/WholeSample.dir/source/stdafx.cpp.o" \
"CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o" \
"CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o" \
"CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o" \
"CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o" \
"CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o" \
"CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o" \
"CMakeFiles/WholeSample.dir/source/EventListener.cpp.o" \
"CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o" \
"CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o"

# External object files for target WholeSample
WholeSample_EXTERNAL_OBJECTS =

WholeSample: CMakeFiles/WholeSample.dir/source/stdafx.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/EventListener.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o
WholeSample: CMakeFiles/WholeSample.dir/build.make
WholeSample: CMakeFiles/WholeSample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable WholeSample"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WholeSample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WholeSample.dir/build: WholeSample
.PHONY : CMakeFiles/WholeSample.dir/build

CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/stdafx.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/WholeSample.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/CreateOrderSample.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/PermissionCheckerSample.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/SimpleLog.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/ResponseListener.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/ResponseQueue.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/EventListener.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/SessionStatusListener.cpp.o.requires
CMakeFiles/WholeSample.dir/requires: CMakeFiles/WholeSample.dir/source/TradingSettingsProviderSample.cpp.o.requires
.PHONY : CMakeFiles/WholeSample.dir/requires

CMakeFiles/WholeSample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WholeSample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WholeSample.dir/clean

CMakeFiles/WholeSample.dir/depend:
	cd /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample /home/adminuser/Documents/forex_development/ForexConnectAPI-1.1.2-Linux-i686/samples/cpp/WholeSample/CMakeFiles/WholeSample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WholeSample.dir/depend

