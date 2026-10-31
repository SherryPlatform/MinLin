##
## PROJECT:    MinLin
## FILE:       Makefile
## PURPOSE:    Makefile for MountHcsPlan9 project
##
## LICENSE:    The MIT License
##
## MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
##

.RECIPEPREFIX := $(subst X, ,X)

# GNU make >= 4.3; GNU-compatible compiler/binutils. Recipes use spaces.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

PROJECT_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIRECTORY := $(patsubst %/,%,$(dir $(PROJECT_MAKEFILE)))
TOOLS_DIRECTORY := $(abspath $(PROJECT_DIRECTORY)/..)
OUTPUT_ROOT ?= $(TOOLS_DIRECTORY)/Output
# Set BOTH for simultaneous toolchains/configurations.
VARIANT ?= default
BINARY_SUFFIX ?=
OBJECT_DIRECTORY := $(OUTPUT_ROOT)/Objects/MountHcsPlan9/$(VARIANT)
BINARY_DIRECTORY := $(OUTPUT_ROOT)/Binaries
PROGRAM := $(BINARY_DIRECTORY)/mounthcs9p$(BINARY_SUFFIX)
SYMBOLS := $(PROGRAM).debug
FULL_PROGRAM := $(OBJECT_DIRECTORY)/mounthcs9p.full
BUILD_OPTIONS := $(OBJECT_DIRECTORY)/build-options.txt
OBJECT := $(OBJECT_DIRECTORY)/MountHcsPlan9.o
TEST_OBJECT := $(OBJECT_DIRECTORY)/TestMountHcsPlan9.o
TEST_PROGRAM := $(OBJECT_DIRECTORY)/TestMountHcsPlan9

# Use a C compiler driver for the final link: no implicit libstdc++.
CC ?= gcc
OBJCOPY ?= objcopy
STRIP ?= strip
CXXFLAGS ?= -O2
CXX_STANDARD ?= c++20
MILE_MOBILITY ?= $(TOOLS_DIRECTORY)/Mile.Mobility
MOBILITY_HEADER := $(MILE_MOBILITY)/Mile.Mobility.Portable.Types.h
# Optional target Linux UAPI include paths for toolchains such as Debian musl.
# Never unconditionally put host glibc headers ahead of the target libc.
UAPI_CPPFLAGS ?=
INCLUDE_FLAGS = -isystem $(MILE_MOBILITY) $(UAPI_CPPFLAGS)
CXX_OPTIONS = $(CXXFLAGS) -std=$(CXX_STANDARD) -g \
    -Wall -Wextra -Wconversion \
    -fno-exceptions -fno-rtti -fno-threadsafe-statics \
    -ffunction-sections -fdata-sections

# Probe the compiler TARGET lazily; clean needs neither compiler nor headers.
CXX_ARCH_FLAGS = $(shell $(CC) $(CPPFLAGS) $(INCLUDE_FLAGS) $(CXX_OPTIONS) \
    -dM -E -x c++ /dev/null | \
    awk '{ Macros[$$2] = 1 } END { \
        if (Macros["__x86_64__"] && !Macros["_M_AMD64"]) \
            print "-D_M_AMD64"; \
        else if (Macros["__aarch64__"] && !Macros["_M_ARM64"]) \
            print "-D_M_ARM64"; \
        else if (Macros["__i386__"] && !Macros["_M_IX86"]) \
            print "-D_M_IX86"; \
    }')

.PHONY: all test clean FORCE
all: $(PROGRAM) $(SYMBOLS)

$(OBJECT_DIRECTORY) $(BINARY_DIRECTORY):
    mkdir -p "$@"

$(BUILD_OPTIONS): FORCE | $(OBJECT_DIRECTORY)
    @printf '%s\n' 'CC=$(CC)' 'CPPFLAGS=$(CPPFLAGS)' \
        'INCLUDE_FLAGS=$(INCLUDE_FLAGS)' 'CXX_OPTIONS=$(CXX_OPTIONS)' \
        'CXX_ARCH_FLAGS=$(CXX_ARCH_FLAGS)' 'LDFLAGS=$(LDFLAGS)' \
        'LDLIBS=$(LDLIBS)' 'OBJCOPY=$(OBJCOPY)' 'STRIP=$(STRIP)' > "$@.tmp"
    @cmp -s "$@.tmp" "$@" || mv -f "$@.tmp" "$@"
    @rm -f "$@.tmp"

# Explicit sources prevent stale outputs from hiding a missing source file.
# -MD includes system/-isystem dependencies, including Portable.Types.
$(OBJECT): $(PROJECT_DIRECTORY)/MountHcsPlan9.cpp $(MOBILITY_HEADER) \
    $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) -x c++ $(CPPFLAGS) $(INCLUDE_FLAGS) $(CXX_OPTIONS) \
        $(CXX_ARCH_FLAGS) -MD -MP -MF "$(@:.o=.d)" -MT "$@" \
        -c -o "$@" "$<"

$(FULL_PROGRAM): $(OBJECT) $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) $(CXXFLAGS) $(LDFLAGS) \
        -Wl,--build-id=sha1 -Wl,--gc-sections \
        -o "$@" $(OBJECT) $(LDLIBS)

# Keep DWARF until after extraction. Recreate the pair if either is missing.
$(PROGRAM) $(SYMBOLS) &: $(FULL_PROGRAM) | $(BINARY_DIRECTORY)
    $(OBJCOPY) --only-keep-debug "$<" "$(SYMBOLS).tmp"
    mv -f "$(SYMBOLS).tmp" "$(SYMBOLS)"
    $(STRIP) --strip-unneeded -o "$(PROGRAM).tmp" "$<"
    $(OBJCOPY) --add-gnu-debuglink="$(SYMBOLS)" "$(PROGRAM).tmp"
    mv -f "$(PROGRAM).tmp" "$(PROGRAM)"

# Optional offline harness. Production all/clean do not require tests/.
$(TEST_OBJECT): $(PROJECT_DIRECTORY)/tests/TestMountHcsPlan9.cpp \
    $(PROJECT_DIRECTORY)/MountHcsPlan9.cpp $(MOBILITY_HEADER) \
    $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) -x c++ $(CPPFLAGS) $(INCLUDE_FLAGS) $(CXX_OPTIONS) \
        $(CXX_ARCH_FLAGS) -MD -MP -MF "$(@:.o=.d)" -MT "$@" \
        -c -o "$@" "$<"

$(TEST_PROGRAM): $(TEST_OBJECT) $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) $(CXXFLAGS) $(LDFLAGS) -Wl,--gc-sections \
        -o "$@" $(TEST_OBJECT) $(LDLIBS)

test: $(TEST_PROGRAM)
    "$(TEST_PROGRAM)"

# Only this project's selected variant and its final pair are removed.
clean:
    rm -rf "$(OBJECT_DIRECTORY)"
    rm -f "$(PROGRAM)" "$(SYMBOLS)" "$(PROGRAM).tmp" "$(SYMBOLS).tmp"

-include $(OBJECT:.o=.d) $(TEST_OBJECT:.o=.d)
