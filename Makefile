TARGET=network_opt

CXX=g++
CXX_FLAGS=-Wall -O3

SRC_DIR=src
BUILD_DIR=build

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
SRCS=$(filter-out $(SRC_DIR)/network_opt_tests.cpp $(SRC_DIR)/network_opt_figures.cpp, $(SRC_FILES))
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR)	
	$(CXX) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r $(OBJS)
	rm $(TARGET)