.PHONY: default run bench test build zip clean


TOP = $(shell pwd)
TOP_BASE = $(shell basename ${TOP})

INC_DIR := $(TOP)/include
SRC_DIR := $(TOP)/src
LIB_DIR := $(TOP)/lib
BIN_DIR := $(TOP)/bin

CXX := g++
CXXFALGS := -I$(INC_DIR) -g -std=c++17 -O3 -fsanitize=address

INCS := $(wildcard $(INC_DIR)/*.h)
LIBS := $(patsubst $(LIB_DIR)/%.cpp,$(LIB_DIR)/%.o,$(wildcard $(LIB_DIR)/*.cpp))
BINS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.out,$(wildcard $(SRC_DIR)/*.cpp))

default: run

run: build
	@for binary in $(BINS); do \
		echo "\e[32mRunning $$binary\e[0m"; \
		$$binary; \
	done

bench: $(BIN_DIR)/bench.out
	$<
test: $(BIN_DIR)/test.out
	$<

build: $(BINS)
$(BIN_DIR)/%.out: $(SRC_DIR)/%.cpp $(LIBS) $(INCS) | $(BIN_DIR)
	-$(CXX) $(CXXFALGS) -o $@ $< $(LIBS)
$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp $(INCS)
	-$(CXX) $(CXXFALGS) -c -o $@ $<
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

zip: $(TOP_BASE).zip
$(TOP_BASE).zip: $(shell find . -type f -not -name "*.zip") clean
	zip -r $(TOP_BASE).zip . -x "*.zip" ".git/*" ".vscode/*"

clean:
	rm -rf $(BIN_DIR) $(LIBS)

compile_flags.txt: Makefile
	echo $(CXX) $(CXXFALGS) | sed 's/\s\+/\n/g' > compile_flags.txt
