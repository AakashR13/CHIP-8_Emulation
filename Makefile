# set the compiler
CXX := g++

# set the compiler flags
CXXFLAGS := `sdl2-config --cflags` -ggdb3 -O0 --std=c++11 -Wall
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lm

# directories
SRC_DIR := src
OBJ_DIR := bin/inter
EXC_DIR := bin

# add header files here
HDRS :=

# add source files here
SRCS := $(SRC_DIR)/main.cpp

# generate names of object files
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# name of executable
EXEC := $(EXC_DIR)/test

# default recipe
all: $(EXEC)

# recipe for building the final executable
$(EXEC): $(OBJS) $(HDRS) Makefile
	@mkdir -p $(EXC_DIR)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

# recipe for building object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HDRS) Makefile
	@mkdir -p $(OBJ_DIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# additional recipes
showfont: showfont.c Makefile
	$(CC) -o $@ $@.c $(CXXFLAGS) $(LDFLAGS)

glfont: glfont.c Makefile
	$(CC) -o $@ $@.c $(CXXFLAGS) $(LDFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

run:
	./$(EXEC)
	
.PHONY: all clean
