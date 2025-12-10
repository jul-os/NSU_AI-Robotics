CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -O2
TARGET = test
SRC = test.cpp

$(TARGET): $(SRC) serialize.h
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: clean