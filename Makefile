# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = "/home/oskar/Programmierprojekte/C*/cstc-cpp"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/oskar/Programmierprojekte/C*/cstc-cpp"

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/cmake-gui -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start "/home/oskar/Programmierprojekte/C*/cstc-cpp/CMakeFiles" "/home/oskar/Programmierprojekte/C*/cstc-cpp//CMakeFiles/progress.marks"
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start "/home/oskar/Programmierprojekte/C*/cstc-cpp/CMakeFiles" 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named cstc

# Build rule for target.
cstc: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 cstc
.PHONY : cstc

# fast build rule for target.
cstc/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/build
.PHONY : cstc/fast

src/build.o: src/build.cpp.o
.PHONY : src/build.o

# target to build an object file
src/build.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/build.cpp.o
.PHONY : src/build.cpp.o

src/build.i: src/build.cpp.i
.PHONY : src/build.i

# target to preprocess a source file
src/build.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/build.cpp.i
.PHONY : src/build.cpp.i

src/build.s: src/build.cpp.s
.PHONY : src/build.s

# target to generate assembly for a file
src/build.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/build.cpp.s
.PHONY : src/build.cpp.s

src/lexer/lexer.o: src/lexer/lexer.cpp.o
.PHONY : src/lexer/lexer.o

# target to build an object file
src/lexer/lexer.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/lexer/lexer.cpp.o
.PHONY : src/lexer/lexer.cpp.o

src/lexer/lexer.i: src/lexer/lexer.cpp.i
.PHONY : src/lexer/lexer.i

# target to preprocess a source file
src/lexer/lexer.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/lexer/lexer.cpp.i
.PHONY : src/lexer/lexer.cpp.i

src/lexer/lexer.s: src/lexer/lexer.cpp.s
.PHONY : src/lexer/lexer.s

# target to generate assembly for a file
src/lexer/lexer.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/lexer/lexer.cpp.s
.PHONY : src/lexer/lexer.cpp.s

src/main.o: src/main.cpp.o
.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i
.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s
.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/module.o: src/module.cpp.o
.PHONY : src/module.o

# target to build an object file
src/module.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/module.cpp.o
.PHONY : src/module.cpp.o

src/module.i: src/module.cpp.i
.PHONY : src/module.i

# target to preprocess a source file
src/module.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/module.cpp.i
.PHONY : src/module.cpp.i

src/module.s: src/module.cpp.s
.PHONY : src/module.s

# target to generate assembly for a file
src/module.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/module.cpp.s
.PHONY : src/module.cpp.s

src/parser/parser.o: src/parser/parser.cpp.o
.PHONY : src/parser/parser.o

# target to build an object file
src/parser/parser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/parser.cpp.o
.PHONY : src/parser/parser.cpp.o

src/parser/parser.i: src/parser/parser.cpp.i
.PHONY : src/parser/parser.i

# target to preprocess a source file
src/parser/parser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/parser.cpp.i
.PHONY : src/parser/parser.cpp.i

src/parser/parser.s: src/parser/parser.cpp.s
.PHONY : src/parser/parser.s

# target to generate assembly for a file
src/parser/parser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/parser.cpp.s
.PHONY : src/parser/parser.cpp.s

src/parser/symboltable.o: src/parser/symboltable.cpp.o
.PHONY : src/parser/symboltable.o

# target to build an object file
src/parser/symboltable.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/symboltable.cpp.o
.PHONY : src/parser/symboltable.cpp.o

src/parser/symboltable.i: src/parser/symboltable.cpp.i
.PHONY : src/parser/symboltable.i

# target to preprocess a source file
src/parser/symboltable.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/symboltable.cpp.i
.PHONY : src/parser/symboltable.cpp.i

src/parser/symboltable.s: src/parser/symboltable.cpp.s
.PHONY : src/parser/symboltable.s

# target to generate assembly for a file
src/parser/symboltable.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/cstc.dir/build.make CMakeFiles/cstc.dir/src/parser/symboltable.cpp.s
.PHONY : src/parser/symboltable.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... cstc"
	@echo "... src/build.o"
	@echo "... src/build.i"
	@echo "... src/build.s"
	@echo "... src/lexer/lexer.o"
	@echo "... src/lexer/lexer.i"
	@echo "... src/lexer/lexer.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/module.o"
	@echo "... src/module.i"
	@echo "... src/module.s"
	@echo "... src/parser/parser.o"
	@echo "... src/parser/parser.i"
	@echo "... src/parser/parser.s"
	@echo "... src/parser/symboltable.o"
	@echo "... src/parser/symboltable.i"
	@echo "... src/parser/symboltable.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

