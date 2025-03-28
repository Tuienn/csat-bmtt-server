CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include
LDFLAGS = -lboost_system -lsqlite3 -lpthread

SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

EXEC = server

# Create build directory if it doesn't exist
$(shell mkdir -p $(OBJ_DIR))

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Rule to compile each source file to an object file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

# Run the server
run: $(EXEC)
	./$(EXEC)
