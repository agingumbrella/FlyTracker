# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jlab/FlyTracker/c++

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jlab/FlyTracker/c++/build

# Include any dependencies generated for this target.
include CMakeFiles/flytracker.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/flytracker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/flytracker.dir/flags.make

CMakeFiles/flytracker.dir/fmf.cpp.o: CMakeFiles/flytracker.dir/flags.make
CMakeFiles/flytracker.dir/fmf.cpp.o: ../fmf.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jlab/FlyTracker/c++/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/flytracker.dir/fmf.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/flytracker.dir/fmf.cpp.o -c /home/jlab/FlyTracker/c++/fmf.cpp

CMakeFiles/flytracker.dir/fmf.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flytracker.dir/fmf.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jlab/FlyTracker/c++/fmf.cpp > CMakeFiles/flytracker.dir/fmf.cpp.i

CMakeFiles/flytracker.dir/fmf.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flytracker.dir/fmf.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jlab/FlyTracker/c++/fmf.cpp -o CMakeFiles/flytracker.dir/fmf.cpp.s

CMakeFiles/flytracker.dir/fmf.cpp.o.requires:
.PHONY : CMakeFiles/flytracker.dir/fmf.cpp.o.requires

CMakeFiles/flytracker.dir/fmf.cpp.o.provides: CMakeFiles/flytracker.dir/fmf.cpp.o.requires
	$(MAKE) -f CMakeFiles/flytracker.dir/build.make CMakeFiles/flytracker.dir/fmf.cpp.o.provides.build
.PHONY : CMakeFiles/flytracker.dir/fmf.cpp.o.provides

CMakeFiles/flytracker.dir/fmf.cpp.o.provides.build: CMakeFiles/flytracker.dir/fmf.cpp.o

CMakeFiles/flytracker.dir/tracker.cpp.o: CMakeFiles/flytracker.dir/flags.make
CMakeFiles/flytracker.dir/tracker.cpp.o: ../tracker.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jlab/FlyTracker/c++/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/flytracker.dir/tracker.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/flytracker.dir/tracker.cpp.o -c /home/jlab/FlyTracker/c++/tracker.cpp

CMakeFiles/flytracker.dir/tracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flytracker.dir/tracker.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jlab/FlyTracker/c++/tracker.cpp > CMakeFiles/flytracker.dir/tracker.cpp.i

CMakeFiles/flytracker.dir/tracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flytracker.dir/tracker.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jlab/FlyTracker/c++/tracker.cpp -o CMakeFiles/flytracker.dir/tracker.cpp.s

CMakeFiles/flytracker.dir/tracker.cpp.o.requires:
.PHONY : CMakeFiles/flytracker.dir/tracker.cpp.o.requires

CMakeFiles/flytracker.dir/tracker.cpp.o.provides: CMakeFiles/flytracker.dir/tracker.cpp.o.requires
	$(MAKE) -f CMakeFiles/flytracker.dir/build.make CMakeFiles/flytracker.dir/tracker.cpp.o.provides.build
.PHONY : CMakeFiles/flytracker.dir/tracker.cpp.o.provides

CMakeFiles/flytracker.dir/tracker.cpp.o.provides.build: CMakeFiles/flytracker.dir/tracker.cpp.o

CMakeFiles/flytracker.dir/main.cpp.o: CMakeFiles/flytracker.dir/flags.make
CMakeFiles/flytracker.dir/main.cpp.o: ../main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jlab/FlyTracker/c++/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/flytracker.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/flytracker.dir/main.cpp.o -c /home/jlab/FlyTracker/c++/main.cpp

CMakeFiles/flytracker.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flytracker.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jlab/FlyTracker/c++/main.cpp > CMakeFiles/flytracker.dir/main.cpp.i

CMakeFiles/flytracker.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flytracker.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jlab/FlyTracker/c++/main.cpp -o CMakeFiles/flytracker.dir/main.cpp.s

CMakeFiles/flytracker.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/flytracker.dir/main.cpp.o.requires

CMakeFiles/flytracker.dir/main.cpp.o.provides: CMakeFiles/flytracker.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/flytracker.dir/build.make CMakeFiles/flytracker.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/flytracker.dir/main.cpp.o.provides

CMakeFiles/flytracker.dir/main.cpp.o.provides.build: CMakeFiles/flytracker.dir/main.cpp.o

CMakeFiles/flytracker.dir/hungarian/asp.cpp.o: CMakeFiles/flytracker.dir/flags.make
CMakeFiles/flytracker.dir/hungarian/asp.cpp.o: ../hungarian/asp.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jlab/FlyTracker/c++/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/flytracker.dir/hungarian/asp.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/flytracker.dir/hungarian/asp.cpp.o -c /home/jlab/FlyTracker/c++/hungarian/asp.cpp

CMakeFiles/flytracker.dir/hungarian/asp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flytracker.dir/hungarian/asp.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jlab/FlyTracker/c++/hungarian/asp.cpp > CMakeFiles/flytracker.dir/hungarian/asp.cpp.i

CMakeFiles/flytracker.dir/hungarian/asp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flytracker.dir/hungarian/asp.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jlab/FlyTracker/c++/hungarian/asp.cpp -o CMakeFiles/flytracker.dir/hungarian/asp.cpp.s

CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.requires:
.PHONY : CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.requires

CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.provides: CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.requires
	$(MAKE) -f CMakeFiles/flytracker.dir/build.make CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.provides.build
.PHONY : CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.provides

CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.provides.build: CMakeFiles/flytracker.dir/hungarian/asp.cpp.o

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o: CMakeFiles/flytracker.dir/flags.make
CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o: ../hungarian/hungarian.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/jlab/FlyTracker/c++/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o -c /home/jlab/FlyTracker/c++/hungarian/hungarian.cpp

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/jlab/FlyTracker/c++/hungarian/hungarian.cpp > CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.i

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/jlab/FlyTracker/c++/hungarian/hungarian.cpp -o CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.s

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.requires:
.PHONY : CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.requires

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.provides: CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.requires
	$(MAKE) -f CMakeFiles/flytracker.dir/build.make CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.provides.build
.PHONY : CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.provides

CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.provides.build: CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o

# Object files for target flytracker
flytracker_OBJECTS = \
"CMakeFiles/flytracker.dir/fmf.cpp.o" \
"CMakeFiles/flytracker.dir/tracker.cpp.o" \
"CMakeFiles/flytracker.dir/main.cpp.o" \
"CMakeFiles/flytracker.dir/hungarian/asp.cpp.o" \
"CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o"

# External object files for target flytracker
flytracker_EXTERNAL_OBJECTS =

flytracker: CMakeFiles/flytracker.dir/fmf.cpp.o
flytracker: CMakeFiles/flytracker.dir/tracker.cpp.o
flytracker: CMakeFiles/flytracker.dir/main.cpp.o
flytracker: CMakeFiles/flytracker.dir/hungarian/asp.cpp.o
flytracker: CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o
flytracker: /usr/local/lib/libopencv_calib3d.so
flytracker: /usr/local/lib/libopencv_contrib.so
flytracker: /usr/local/lib/libopencv_core.so
flytracker: /usr/local/lib/libopencv_features2d.so
flytracker: /usr/local/lib/libopencv_flann.so
flytracker: /usr/local/lib/libopencv_gpu.so
flytracker: /usr/local/lib/libopencv_highgui.so
flytracker: /usr/local/lib/libopencv_imgproc.so
flytracker: /usr/local/lib/libopencv_legacy.so
flytracker: /usr/local/lib/libopencv_ml.so
flytracker: /usr/local/lib/libopencv_nonfree.so
flytracker: /usr/local/lib/libopencv_objdetect.so
flytracker: /usr/local/lib/libopencv_photo.so
flytracker: /usr/local/lib/libopencv_stitching.so
flytracker: /usr/local/lib/libopencv_superres.so
flytracker: /usr/local/lib/libopencv_ts.so
flytracker: /usr/local/lib/libopencv_video.so
flytracker: /usr/local/lib/libopencv_videostab.so
flytracker: CMakeFiles/flytracker.dir/build.make
flytracker: CMakeFiles/flytracker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable flytracker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/flytracker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/flytracker.dir/build: flytracker
.PHONY : CMakeFiles/flytracker.dir/build

CMakeFiles/flytracker.dir/requires: CMakeFiles/flytracker.dir/fmf.cpp.o.requires
CMakeFiles/flytracker.dir/requires: CMakeFiles/flytracker.dir/tracker.cpp.o.requires
CMakeFiles/flytracker.dir/requires: CMakeFiles/flytracker.dir/main.cpp.o.requires
CMakeFiles/flytracker.dir/requires: CMakeFiles/flytracker.dir/hungarian/asp.cpp.o.requires
CMakeFiles/flytracker.dir/requires: CMakeFiles/flytracker.dir/hungarian/hungarian.cpp.o.requires
.PHONY : CMakeFiles/flytracker.dir/requires

CMakeFiles/flytracker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/flytracker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/flytracker.dir/clean

CMakeFiles/flytracker.dir/depend:
	cd /home/jlab/FlyTracker/c++/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jlab/FlyTracker/c++ /home/jlab/FlyTracker/c++ /home/jlab/FlyTracker/c++/build /home/jlab/FlyTracker/c++/build /home/jlab/FlyTracker/c++/build/CMakeFiles/flytracker.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/flytracker.dir/depend

