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
CMAKE_SOURCE_DIR = /home/pascalr/invention/vision

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pascalr/invention/vision

# Include any dependencies generated for this target.
include CMakeFiles/scan_qr_code.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/scan_qr_code.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/scan_qr_code.dir/flags.make

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o: CMakeFiles/scan_qr_code.dir/flags.make
CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o: scan_qr_code.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pascalr/invention/vision/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o -c /home/pascalr/invention/vision/scan_qr_code.cpp

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pascalr/invention/vision/scan_qr_code.cpp > CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.i

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pascalr/invention/vision/scan_qr_code.cpp -o CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.s

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.requires:

.PHONY : CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.requires

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.provides: CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.requires
	$(MAKE) -f CMakeFiles/scan_qr_code.dir/build.make CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.provides.build
.PHONY : CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.provides

CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.provides.build: CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o


# Object files for target scan_qr_code
scan_qr_code_OBJECTS = \
"CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o"

# External object files for target scan_qr_code
scan_qr_code_EXTERNAL_OBJECTS =

bin/scan_qr_code: CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o
bin/scan_qr_code: CMakeFiles/scan_qr_code.dir/build.make
bin/scan_qr_code: /usr/local/lib/libopencv_dnn.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_gapi.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_highgui.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_ml.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_objdetect.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_photo.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_stitching.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_video.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_videoio.so.4.3.0
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_program_options.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_log.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_log_setup.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_system.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_thread.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_regex.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libpthread.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_log.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_log_setup.so
bin/scan_qr_code: /usr/local/lib/libopencv_imgcodecs.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_calib3d.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_features2d.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_flann.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_imgproc.so.4.3.0
bin/scan_qr_code: /usr/local/lib/libopencv_core.so.4.3.0
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_system.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_thread.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_regex.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
bin/scan_qr_code: /usr/lib/x86_64-linux-gnu/libpthread.so
bin/scan_qr_code: CMakeFiles/scan_qr_code.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pascalr/invention/vision/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable bin/scan_qr_code"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/scan_qr_code.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/scan_qr_code.dir/build: bin/scan_qr_code

.PHONY : CMakeFiles/scan_qr_code.dir/build

CMakeFiles/scan_qr_code.dir/requires: CMakeFiles/scan_qr_code.dir/scan_qr_code.cpp.o.requires

.PHONY : CMakeFiles/scan_qr_code.dir/requires

CMakeFiles/scan_qr_code.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/scan_qr_code.dir/cmake_clean.cmake
.PHONY : CMakeFiles/scan_qr_code.dir/clean

CMakeFiles/scan_qr_code.dir/depend:
	cd /home/pascalr/invention/vision && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pascalr/invention/vision /home/pascalr/invention/vision /home/pascalr/invention/vision /home/pascalr/invention/vision /home/pascalr/invention/vision/CMakeFiles/scan_qr_code.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/scan_qr_code.dir/depend

