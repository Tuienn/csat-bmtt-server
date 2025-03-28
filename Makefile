CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include
LDFLAGS = -lboost_system -lsqlite3 -lpthread

SRC_DIR = src
INCLUDE_DIR = include
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/server.cpp $(SRC_DIR)/database.cpp $(SRC_DIR)/users.cpp
EXEC = server

$(EXEC): $(SOURCES)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(EXEC)
