CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I./library/include -I./library

LIBS = -lssl -lcrypto
GTEST_LIBS = -lgtest -lgtest_main -lpthread

TARGET = programa
TEST_TARGET = tests_runner

SRC = example/main.cpp $(shell find library -name "*.cpp")
OBJ = $(SRC:.cpp=.o)

LIB_OBJ = $(filter-out example/main.o, $(OBJ))

TEST_SRC = $(shell find tests -name "*.cpp")
TEST_OBJ = $(TEST_SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LIBS)

# Tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ) $(LIB_OBJ)
	$(CXX) $(TEST_OBJ) $(LIB_OBJ) -o $(TEST_TARGET) $(LIBS) $(GTEST_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

re: clean all
