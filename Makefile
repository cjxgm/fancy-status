CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -ggdb

CPPS := $(wildcard src/*.cpp)
HPPS := $(wildcard src/*.hpp)

PRELUDE_HPP := src/prelude/prelude.hpp
PRELUDE_CPPS := $(wildcard src/prelude/*.cpp)
PRELUDE_HPPS := $(wildcard src/prelude/*.hpp)

all: build/fancy-status
clean:
	rm -rf build/
run: all
	build/fancy-status

build/build.flags: Makefile | build/
	@printf "%s\n" "Generating build flags..."
	@printf "%s\n" "$(CXXFLAGS) -include $(realpath $(PRELUDE_HPP))" > $@
build/prelude.o: $(PRELUDE_CPPS) $(PRELUDE_HPPS) Makefile | build/
	$(CXX) $(CXXFLAGS) -r -o $@ $(PRELUDE_CPPS)
build/fancy-status: build/build.flags build/prelude.o $(CPPS) $(HPPS) $(PRELUDE_CPPS) $(PRELUDE_HPPS) Makefile | build/
	$(CXX) $(CXXFLAGS) -include $(PRELUDE_HPP) -o $@ $(CPPS) build/prelude.o

.PRECIOUS: %/
%/:
	mkdir -p $@

