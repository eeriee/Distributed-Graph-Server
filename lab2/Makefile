# Author: Xi Wang
#-----------------------------------------------------------
# Macro definitions
CXXFLAGS = -O1 -g -Wall -std=c++1y
OBJ = main.o frozen.o mongoose.o Graph.o Controller.o
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
main.o: main.cpp Controller.hpp
Controller.o: Controller.cpp Graph.hpp SuperBlock.hpp Log.hpp
Graph.o: Graph.cpp Graph.hpp tools.hpp
frozen.o: frozen.c frozen.h
mongoose.o: mongoose.c mongoose.h

