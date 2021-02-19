$(info -- SCAN --)
CXX = g++
BUILD_FLAGS = -std=c++17 -O3 -Wall -Wextra -Werror=switch -Werror=return-type -ggdb
CXXFLAGS = $(BUILD_FLAGS) -r
LDFLAGS = $(BUILD_FLAGS) -Ur -fuse-ld=lld
PRELUDE_HPP := src/prelude/prelude.hpp

.PHONY: all clean run
all: build/fancy-status build/compile_commands.json
clean:
	rm -rf build/
run: all
	build/fancy-status -t
	build/fancy-status

OBJECTS := # Will be filled later
COMPILE_COMMANDS := # Will be filled later

eq = $(and $(findstring .$1,.$2),$(findstring .$2,.$1))

hide-unless-error = @printf "%s\n" "$1"; $2 || { printf "\n\nFAIL %s\n\n" "$2"; false; }

define compile-command-for

	{
		"directory": "$(CURDIR)",
		"output": "$1",
		"file": "$2",
		"command": "$3"
	},
endef

compile-commands-bad-end := ,]
define compile-commands-good-end

]


endef

define compile-target
$2: $3 Makefile | build/
	$(call hide-unless-error,BUILDING $1...,$(CXX) $(CXXFLAGS) -o $$@ $4)

define COMPILE_COMMANDS +=
$(foreach source,$(filter %.cpp,$3),$(call compile-command-for,$2,$(source),$(CXX) $(CXXFLAGS) -o $2 $4))
endef
endef

define link-target
$2: $3 Makefile | build/
	$(call hide-unless-error,LINKING $1...,$(CXX) $(LDFLAGS) -o $$@ $4)

define COMPILE_COMMANDS +=
$(foreach source,$(filter %.o,$3),$(call compile-command-for,$2,$(source),$(CXX) $(LDFLAGS) -o $2 $4))
endef
endef

define stamp-target
$2: $3 Makefile | build/
	$$(info STAMPING $1...)
	$$(file > $$@)

define COMPILE_COMMANDS +=
$(foreach source,$(filter %.hpp %.inl,$3),$(call compile-command-for,$4,$(source),$(CXX) $(CXXFLAGS) -o $4 $5))
endef
endef

define build-directory
CPPS := $$(sort $$(wildcard $(1)*.cpp))
HPPS := $$(sort $$(wildcard $(1)*.hpp $(1)*.inl))

TARGET_NAME := $$(notdir $$(abspath $1))
TARGET_OBJ := build/$$(TARGET_NAME).o
TARGET_STAMP := build/$$(TARGET_NAME).stamp

IS_PRELUDE := $$(call eq,$$(TARGET_NAME),prelude)
INCLUDE_PRELUDE := $$(if $$(IS_PRELUDE),,-include $(PRELUDE_HPP))

DEP_NAMES := $$(filter-out prelude,$$(notdir $$(realpath $$(sort $$(wildcard $(1)*/)))))
DEP_NAMES += $$(if $$(IS_PRELUDE),,prelude)
DEP_STAMPS := $$(patsubst %,build/%.stamp,$$(DEP_NAMES))

$$(info $$(TARGET_NAME) REQUIRES $$(or $$(DEP_NAMES),NOTHING))
$$(info $$(TARGET_NAME) COMPILES $$(or $$(CPPS),NOTHING))

ifneq "$$(strip $$(CPPS))" ""
OBJECTS += $$(TARGET_OBJ)

# Add another layer of indirection so that variables in shell commands are expanded right now.
$$(eval $$(call compile-target,$$(TARGET_NAME),$$(TARGET_OBJ),$$(CPPS) $$(TARGET_STAMP) $$(DEP_STAMPS),$$(INCLUDE_PRELUDE) $$(CPPS)))
endif

$$(eval $$(call stamp-target,$$(TARGET_NAME),$$(TARGET_STAMP),$$(HPPS),$$(TARGET_OBJ),$$(INCLUDE_PRELUDE) $$(or $$(CPPS),$$(HPPS))))
endef

MODULES := src/
MODULES += $(sort $(wildcard src/*/))
$(foreach module,$(MODULES),$(eval $(call build-directory,$(module))))

$(eval $(call link-target,fancy-status,build/fancy-status,$(OBJECTS),$(OBJECTS)))

.PHONY: build/compile_commands.json
build/compile_commands.json: | build/
	$(file > $@,$(subst $(compile-commands-bad-end),$(compile-commands-good-end),[$(COMPILE_COMMANDS)]))

.PRECIOUS: %/
%/:
	$(call hide-unless-error,MAKING DIRECTORY $*...,mkdir -p $@)

$(info -- BUILD --)

