# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.2

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
CMAKE_SOURCE_DIR = /home/michelo/progetti/chaos_bundle/driver/powersupply

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michelo/progetti/chaos_bundle/driver/powersupply

# Include any dependencies generated for this target.
include models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/depend.make

# Include the progress variables for this target.
include models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/progress.make

# Include the compile flags for this target's objects.
include models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/flags.make

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/flags.make
models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o: models/Ocem/OcemDD.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/michelo/progetti/chaos_bundle/driver/powersupply/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o"
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o -c /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem/OcemDD.cpp

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.i"
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem/OcemDD.cpp > CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.i

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.s"
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem/OcemDD.cpp -o CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.s

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.requires:
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.requires

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.provides: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.requires
	$(MAKE) -f models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/build.make models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.provides.build
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.provides

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.provides.build: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o

# Object files for target chaos_driver_powersupply_ocem_dd
chaos_driver_powersupply_ocem_dd_OBJECTS = \
"CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o"

# External object files for target chaos_driver_powersupply_ocem_dd
chaos_driver_powersupply_ocem_dd_EXTERNAL_OBJECTS =

build/libchaos_driver_powersupply_ocem_dd.so: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o
build/libchaos_driver_powersupply_ocem_dd.so: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/build.make
build/libchaos_driver_powersupply_ocem_dd.so: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library ../../build/libchaos_driver_powersupply_ocem_dd.so"
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/build: build/libchaos_driver_powersupply_ocem_dd.so
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/build

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/requires: models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/OcemDD.cpp.o.requires
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/requires

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/clean:
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem && $(CMAKE_COMMAND) -P CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/cmake_clean.cmake
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/clean

models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/depend:
	cd /home/michelo/progetti/chaos_bundle/driver/powersupply && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michelo/progetti/chaos_bundle/driver/powersupply /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem /home/michelo/progetti/chaos_bundle/driver/powersupply /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem /home/michelo/progetti/chaos_bundle/driver/powersupply/models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : models/Ocem/CMakeFiles/chaos_driver_powersupply_ocem_dd.dir/depend

