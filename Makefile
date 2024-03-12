.PHONY: default run build clean

SRC_DIR := src
BIN_DIR := bin

CXX := g++
CXXFALGS := -I$(SRC_DIR) -g -std=c++17 -O0 -fsanitize=address

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
INCS := $(wildcard $(SRC_DIR)/*.h)
BINS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.out,$(SRCS))

default: run

run: build
	@for binary in $(BINS); do \
		echo "\e[32mRunning $$binary\e[0m"; \
		$$binary; \
	done

build: $(BINS)
$(BIN_DIR)/%.out: $(SRC_DIR)/%.cpp $(INCS) | $(BIN_DIR)
	-$(CXX) $(CXXFALGS) -o $@ $<
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

compile_flags.txt: Makefile
	echo $(CXX) $(CXXFALGS) | sed 's/ /\n/g' > compile_flags.txt
