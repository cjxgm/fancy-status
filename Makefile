CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra

SRCS := $(wildcard src/*.cpp)

all: build/fancy-status
clean:
	rm -rf build/
run: all
	build/fancy-status

build/fancy-status: $(SRCS) | build/
	$(CXX) $(CXXFLAGS) -o $@ $(SRCS)

.PRECIOUS: %/
%/:
	mkdir -p $@

