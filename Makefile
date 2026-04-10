# Compiler
CXX = g++

# C++ standard
CXXSTD = c++17

# Compilation flags
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -Wpedantic

# Debug flags
DEBUG_FLAGS = -g -O0

# Release flags
RELEASE_FLAGS = -O3 -DNDEBUG

# Include flags
INCLUDE_FLAGS = -I. -Isequence

# Targets
.PHONY: all debug release clean run run_release test help

# Executable names
DEBUG_TARGET = piecewise_debug
RELEASE_TARGET = piecewise_release
TEST_TARGET = piecewise_test.exe

SOURCES = main.cpp
TEST_SOURCES = tests.cpp

# Template implementation files
TEMPLATE_FILES = interval.tpp function_piece.tpp piecewise_function.tpp \
                 functions/power_function.tpp functions/exponential_function.tpp \
                 functions/logarithm_function.tpp functions/sine_function.tpp \
                 functions/cosine_function.tpp \
                 sequence/immutable_array_sequence.tpp sequence/mutable_array_sequence.tpp \
                 sequence/immutable_list_sequence.tpp sequence/mutable_list_sequence.tpp \
                 sequence/linked_list.tpp sequence/dynamic_array.tpp \
                 sequence/sequence.tpp sequence/array_sequence.tpp \
                 sequence/list_sequence.tpp sequence/sequence_functions.tpp

# Header files
HEADERS = function.h interval.h function_piece.h piecewise_function.h \
          functions/power_function.h functions/exponential_function.h \
          functions/logarithm_function.h functions/sine_function.h \
          functions/cosine_function.h \
          sequence/immutable_array_sequence.h sequence/mutable_array_sequence.h \
          sequence/immutable_list_sequence.h sequence/mutable_list_sequence.h \
          sequence/linked_list.h sequence/dynamic_array.h sequence/sequence.h \
          sequence/array_sequence.h sequence/list_sequence.h \
          sequence/IEnumerator.h sequence/sequence_functions.h

# Full dependency list
DEPS = $(SOURCES) $(HEADERS) $(TEMPLATE_FILES)
TEST_DEPS = $(TEST_SOURCES) $(HEADERS) $(TEMPLATE_FILES)

GTEST_DIR = sequence/googletest/googletest
GTEST_INC = -I$(GTEST_DIR)/include -I$(GTEST_DIR) -Isequence -I.

gtest-all.o:
	$(CXX) -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o:
	$(CXX) -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest_main.cc

tests.o: $(TEST_DEPS)
	$(CXX) -std=$(CXXSTD) $(GTEST_INC) -c $(TEST_SOURCES)

$(TEST_TARGET): tests.o gtest-all.o gtest_main.o
	$(CXX) tests.o gtest-all.o gtest_main.o -o $(TEST_TARGET) -lpthread

# Default target
all: debug release

# Build debug version
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

# Build release version
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_TARGET)

# Compile debug version
$(DEBUG_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

# Compile release version
$(RELEASE_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

# Run debug version
run: $(DEBUG_TARGET)
	./$(DEBUG_TARGET)

# Run release version
run_release: $(RELEASE_TARGET)
	./$(RELEASE_TARGET)

# Run tests
test: $(TEST_TARGET)
	@echo "Running Google Tests..."
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -f $(DEBUG_TARGET) $(RELEASE_TARGET) $(TEST_TARGET) *.o core

# Help
help:
	@echo "Makefile for the piecewise function lab"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - build both debug and release versions"
	@echo "  debug        - build with debugging information"
	@echo "  release      - build optimized version"
	@echo "  run          - compile debug version and run"
	@echo "  run_release  - run release version"
	@echo "  test         - compile and run Google Tests"
	@echo "  clean        - remove compiled files"
	@echo "  help         - display this help message"
