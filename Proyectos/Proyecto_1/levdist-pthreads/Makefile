# Compiler options
CC=gcc
CXX=g++
FLAGS=-Wall -Wextra -pthread
CFLAGS=$(FLAGS)
CXXFLAGS=$(FLAGS)
LIBS=

# Configure flags according to the target
debug: FLAGS += -g
release: FLAGS += -O3 -DNDEBUG

# Directories
BIN_DIR=bin
BUILD_DIR=build
DOC_DIR=doc
SRC_DIR=src
TEST_DIR=test
TEST_OUT_DIR=$(BUILD_DIR)/$(TEST_DIR)
INST_DIR=$(HOME)/bin

# Project variables
APP_NAME=levdist
#EXECUTABLE=$(BIN_DIR)/$(shell basename `pwd`)
EXECUTABLE=$(BIN_DIR)/$(APP_NAME)

# Files and test cases
HEADERS=$(wildcard $(SRC_DIR)/*.h)
SOURCES=$(wildcard $(SRC_DIR)/*.c*)
OBJECTS=$(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TESTS=$(wildcard $(TEST_DIR)/*)
TESTS_OUT=$(TESTS:$(TEST_DIR)/%=$(TEST_OUT_DIR)/%)
TESTS_MEMCHECK=$(TESTS:$(TEST_DIR)/%=$(TEST_OUT_DIR)/%_memcheck)

# Default rule is debug target
debug: $(EXECUTABLE)
release: $(EXECUTABLE)

# Linker call
$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile C source into .o object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

# Compile C++ source into .o object file
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Documentation
.PHONY: doc | $(DOC_DIR)
doc:
	doxygen Doxyfile

# Testing
.PHONY: test
test: debug $(TESTS_OUT) | $(TEST_OUT_DIR)
	@echo "$(testcnt) cases tested, for details call 'make test -n'"

.PHONY: $(TEST_OUT_DIR)/%
$(TEST_OUT_DIR)/%: $(TEST_DIR)/% | icdiff
	@mkdir -p $@ ;\
	cd $< ;\
	../../$(EXECUTABLE) $(shell cat $</args.txt) < input.txt > ../../$@/output.txt 2> ../../$@/error.txt || true ;\
	cd ../.. ;\
	$(HOME)/bin/icdiff $</output.txt $@/output.txt || true ;\
	$(HOME)/bin/icdiff $</error.txt $@/error.txt || true ;\
	rm -rf $@ ;\
	$(eval testcnt=$(shell echo $$(($(testcnt)+1))))

# Profiling
.PHONY: memcheck
memcheck: debug $(TESTS_MEMCHECK)

.PHONY: $(TEST_OUT_DIR)/%_memcheck
$(TEST_OUT_DIR)/%_memcheck: $(TEST_DIR)/%
	valgrind --tool=memcheck $(EXECUTABLE) -Q $(shell cat $</args.txt) < $</input.txt 2>&1 > /dev/null | grep 'lost' || true

# All targets
all: debug doc test

# Remove output directories
.PHONY: clean
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR) $(DOC_DIR)

# Install a copy of the executable in ~/bin
install: release | $(INST_DIR)
	cp -p $(EXECUTABLE) $(INST_DIR)

# Uninstall the copy of the executable in ~/bin
.PHONY: uninstall
uninstall:
	rm -f $(INST_DIR)/$(APP_NAME)


# Rules for creating output directories

$(BIN_DIR):
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

$(TEST_OUT_DIR):
	mkdir -p $@

$(INST_DIR):
	mkdir -p $@

icdiff: $(HOME)/bin/icdiff

$(HOME)/bin/icdiff:
	mkdir -p $(HOME)/bin
	wget -q https://raw.githubusercontent.com/jeffkaufman/icdiff/release-1.9.2/icdiff -O $@
	chmod +x $@
