CXX = g++
BUILD_FLAGS = -std=c++17 -O3 -Wall -Wextra -ggdb
CXXFLAGS = $(BUILD_FLAGS) -r
LDFLAGS = $(BUILD_FLAGS) -Ur -fuse-ld=lld
PRELUDE_HPP := src/prelude/prelude.hpp

.PHONY: all clean run
all: build/fancy-status
clean:
	rm -rf build/
run: all
	build/fancy-status

build/build.flags: Makefile | build/
	@printf "%s\n" "Generating build flags..."
	@printf "%s\n" "$(CXXFLAGS) -include $(realpath $(PRELUDE_HPP))" > $@

OBJECTS := # Will be filled later

eq = $(and $(findstring .$1,.$2),$(findstring .$2,.$1))

hide-unless-error = @printf "%s\n" "$1"; $2 || { printf "\n\nFAIL %s\n\n" "$2"; false; }

define compile-target
$2: $3 Makefile | build/
	$(call hide-unless-error,BUILDING $1...,$(CXX) $(CXXFLAGS) -o $$@ $4)
endef

define link-target
$2: $3 Makefile | build/
	$(call hide-unless-error,LINKING $1...,$(CXX) $(LDFLAGS) -o $$@ $4)
endef

define build-directory
CPPS := $$(wildcard $(1)*.cpp)
HPPS := $$(wildcard $(1)*.hpp)

TARGET_NAME := $$(notdir $$(abspath $1))
TARGET_OBJ := build/$$(TARGET_NAME).o
TARGET_STAMP := build/$$(TARGET_NAME).stamp
OBJECTS += $$(TARGET_OBJ)

IS_PRELUDE := $$(call eq,$$(TARGET_NAME),prelude)
INCLUDE_PRELUDE := $$(if $$(IS_PRELUDE),,-include $(PRELUDE_HPP))

DEP_NAMES := $$(filter-out prelude,$$(notdir $$(realpath $$(wildcard $(1)*/))))
DEP_NAMES += $$(if $$(IS_PRELUDE),,prelude)
DEP_STAMPS := $$(patsubst %,build/%.stamp,$$(DEP_NAMES))

$$(info $$(TARGET_NAME) REQUIRES $$(or $$(DEP_NAMES),NOTHING))
$$(info $$(TARGET_NAME) COMPILES $$(CPPS))

# Add another layer of indirection so that variables in shell commands are expanded right now.
$$(eval $$(call compile-target,$$(TARGET_NAME),$$(TARGET_OBJ),$$(CPPS) $$(TARGET_STAMP) $$(DEP_STAMPS),$$(INCLUDE_PRELUDE) $$(CPPS)))

$$(TARGET_STAMP): $$(HPPS) | build/
	$$(file > $$@)
endef

MODULES := src/
MODULES += $(wildcard src/*/)
$(foreach module,$(MODULES),$(eval $(call build-directory,$(module))))

$(eval $(call link-target,fancy-status,build/fancy-status,build/build.flags $(OBJECTS),$(OBJECTS)))

.PRECIOUS: %/
%/:
	$(call hide-unless-error,MAKING DIRECTORY $*...,mkdir -p $@)

