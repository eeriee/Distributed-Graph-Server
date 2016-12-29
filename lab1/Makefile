# Author: Xi Wang
#-----------------------------------------------------------
# Macro definitions
CXXFLAGS = -O1 -g -Wall -std=c++14
OBJ = main.o frozen.o mongoose.o Graph.o
TARGET = cs426_graph_server
#-----------------------------------------------------------
# Rules
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ)
clean:
	rm -f $(OBJ) $(TARGET)
#-----------------------------------------------------------
# Dependencies
main.o: main.cpp mongoose.h frozen.h Graph.hpp
frozen.o: frozen.c frozen.h
mongoose.o: mongoose.c mongoose.h
Graph.o: Graph.cpp Graph.hpp
