CXX		  := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb
TEST_FLAG := -DTEST

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

EXECUTABLE	:= main


all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) $(shell pkg-config --cflags openssl) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(shell pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib $(shell pkg-config --libs openssl)

clean:
	-rm $(BIN)/*

