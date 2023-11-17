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
	$(CXX) $(CXX_FLAGS) $(TEST_FLAG) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(shell pkg-config --cflags --libs libmongocxx) -Wl,-rpath,/usr/local/lib

clean:
	-rm $(BIN)/*
