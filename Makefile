TARGET=network_opt
TEST_TARGET=network_opt_tests

CXX=g++
CXX_FLAGS=-Wall -O3

SRC_DIR=src
BUILD_DIR=build

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
SRCS=$(filter-out $(SRC_DIR)/network_opt_tests.cpp $(SRC_DIR)/network_opt_figures.cpp, $(SRC_FILES))
OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
TEST_SRCS=$(filter-out $(SRC_DIR)/network_opt_main.cpp $(SRC_DIR)/network_opt_figures.cpp, $(SRC_FILES))
TEST_OBJS=$(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(OBJS) -o $@

$(TEST_TARGET): $(TEST_OBJS) $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(BUILD_DIR) 
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -r $(OBJS)
	rm $(TARGET)
