.PHONY: run build clean

SRC_DIR := .
BIN_DIR := bin

CXX := g++
CXXFALGS := -I$(SRC_DIR) -g -std=c++17 -O0 -fsanitize=address

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
BINS := $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.out,$(SRCS))

run: build
	@for binary in $(BINS); do \
		echo "Running $$binary"; \
		$$binary; \
	done

build: $(BINS)

$(BIN_DIR)/%.out: $(SRC_DIR)/%.cpp | $(BIN_DIR)
	-$(CXX) $(CXXFALGS) -o $@ $<

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

compile_flags.txt: Makefile
	echo $(CXX) $(CXXFALGS) | sed 's/ /\n/g' > compile_flags.txt

clean:
	rm -rf $(BIN_DIR)
