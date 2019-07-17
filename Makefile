SRC_PATH = src/
BIN_PATH = bin/

TARGET = $(BIN_PATH)huffman

$(TARGET): $(SRC_PATH)huffman.cpp
	g++ $^ -o $@
