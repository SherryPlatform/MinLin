##
## PROJECT:    MinLin
## FILE:       Makefile
## PURPOSE:    Makefile for SynthRdp project
##
## LICENSE:    The MIT License
##
## MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
##

.RECIPEPREFIX := $(subst X, ,X)

# Requires GNU make >= 4.3 and GNU-compatible compiler/binutils tools.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

PROJECT_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIRECTORY := $(patsubst %/,%,$(dir $(PROJECT_MAKEFILE)))
TOOLS_DIRECTORY := $(abspath $(PROJECT_DIRECTORY)/..)
OUTPUT_ROOT ?= $(TOOLS_DIRECTORY)/Output
# Separate simultaneous toolchains/configurations with BOTH of these options.
VARIANT ?= default
BINARY_SUFFIX ?=
OBJECT_DIRECTORY := $(OUTPUT_ROOT)/Objects/SynthRdp/$(VARIANT)
BINARY_DIRECTORY := $(OUTPUT_ROOT)/Binaries
PROGRAM := $(BINARY_DIRECTORY)/synthrdp$(BINARY_SUFFIX)
SYMBOLS := $(PROGRAM).debug
FULL_PROGRAM := $(OBJECT_DIRECTORY)/synthrdp.full
BUILD_OPTIONS := $(OBJECT_DIRECTORY)/build-options.txt

# Use a C compiler driver, including for the final link: no implicit libstdc++.
CC ?= gcc
OBJCOPY ?= objcopy
STRIP ?= strip
CFLAGS ?= -O2
CXXFLAGS ?= -O2
# Match the formal vcxproj; the source also remains usable with C++11.
CXX_STANDARD ?= c++20
MILE_MOBILITY := $(TOOLS_DIRECTORY)/Mile.Mobility
MILE_HYPERV := $(TOOLS_DIRECTORY)/Mile.HyperV
INCLUDE_FLAGS := -I$(PROJECT_DIRECTORY) \
                 -isystem $(MILE_MOBILITY) -isystem $(MILE_HYPERV)
C_OPTIONS = $(CFLAGS) -std=c11 -g \
    -Wall -Wextra -Wconversion \
    -ffunction-sections -fdata-sections
CXX_OPTIONS = $(CXXFLAGS) -std=$(CXX_STANDARD) -g \
    -Wall -Wextra -Wconversion -Wno-multichar \
    -fno-exceptions -fno-rtti -fno-threadsafe-statics \
    -ffunction-sections -fdata-sections

# Infer the compiler TARGET, not uname/build-host architecture. Lazy expansion
# lets clean work without a compiler. Explicit target macros are respected.
define ArchitectureFlags
$(shell $(CC) $(CPPFLAGS) $(1) -dM -E -x $(2) /dev/null | \
    awk '{ Macros[$$2] = 1 } END { \
        if (Macros["__x86_64__"] && !Macros["_M_AMD64"]) \
            print "-D_M_AMD64"; \
        else if (Macros["__aarch64__"] && !Macros["_M_ARM64"]) \
            print "-D_M_ARM64"; \
        else if (Macros["__i386__"] && !Macros["_M_IX86"]) \
            print "-D_M_IX86"; \
    }')
endef
C_ARCH_FLAGS = $(call ArchitectureFlags,$(C_OPTIONS),c)
CXX_ARCH_FLAGS = $(call ArchitectureFlags,$(CXX_OPTIONS),c++)

OBJECTS := $(addprefix $(OBJECT_DIRECTORY)/,\
    SynthRdp.o VmbusPipe.o VmbusUio.o Mile.Mobility.Utilities.MemoryAccess.o)

.PHONY: all clean FORCE
all: $(PROGRAM) $(SYMBOLS)

$(OBJECT_DIRECTORY) $(BINARY_DIRECTORY):
    mkdir -p "$@"

# Content comparison prevents stale flags/toolchains without rebuilding on
# an unchanged invocation. These generated files stay under Output/Objects.
$(BUILD_OPTIONS): FORCE | $(OBJECT_DIRECTORY)
    @printf '%s\n' 'CC=$(CC)' 'CPPFLAGS=$(CPPFLAGS)' \
        'INCLUDE_FLAGS=$(INCLUDE_FLAGS)' 'C_OPTIONS=$(C_OPTIONS)' \
        'C_ARCH_FLAGS=$(C_ARCH_FLAGS)' 'CXX_OPTIONS=$(CXX_OPTIONS)' \
        'CXX_ARCH_FLAGS=$(CXX_ARCH_FLAGS)' 'LDFLAGS=$(LDFLAGS)' \
        'LDLIBS=$(LDLIBS)' 'OBJCOPY=$(OBJCOPY)' 'STRIP=$(STRIP)' > "$@.tmp"
    @cmp -s "$@.tmp" "$@" || mv -f "$@.tmp" "$@"
    @rm -f "$@.tmp"

# -MD also tracks the Mile headers passed with -isystem, not just local headers.
$(OBJECT_DIRECTORY)/%.o: $(PROJECT_DIRECTORY)/%.cpp \
                       $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) -x c++ $(CPPFLAGS) $(INCLUDE_FLAGS) $(CXX_OPTIONS) \
        $(CXX_ARCH_FLAGS) -MD -MP -MF "$(@:.o=.d)" -MT "$@" \
        -c -o "$@" "$<"

$(OBJECT_DIRECTORY)/Mile.Mobility.Utilities.MemoryAccess.o: \
    $(MILE_MOBILITY)/Mile.Mobility.Utilities.MemoryAccess.c \
    $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) -x c $(CPPFLAGS) $(INCLUDE_FLAGS) $(C_OPTIONS) $(C_ARCH_FLAGS) \
        -MD -MP -MF "$(@:.o=.d)" -MT "$@" -c -o "$@" "$<"

$(FULL_PROGRAM): $(OBJECTS) $(PROJECT_MAKEFILE) $(BUILD_OPTIONS)
    $(CC) $(CXXFLAGS) $(LDFLAGS) \
        -Wl,--build-id=sha1 -Wl,--gc-sections \
        -o "$@" $(OBJECTS) $(LDLIBS)

# A grouped rule creates a matching pair once, including under make -j.
# Do not pass -s/--strip-all to the linker: extract the DWARF before stripping.
# debuglink records the final basename plus CRC, not an absolute debug path.
$(PROGRAM) $(SYMBOLS) &: $(FULL_PROGRAM) | $(BINARY_DIRECTORY)
    $(OBJCOPY) --only-keep-debug "$<" "$(SYMBOLS).tmp"
    mv -f "$(SYMBOLS).tmp" "$(SYMBOLS)"
    $(STRIP) --strip-unneeded -o "$(PROGRAM).tmp" "$<"
    $(OBJCOPY) --add-gnu-debuglink="$(SYMBOLS)" "$(PROGRAM).tmp"
    mv -f "$(PROGRAM).tmp" "$(PROGRAM)"

# Remove only this project's selected variant; leave other projects untouched.
clean:
    rm -rf "$(OBJECT_DIRECTORY)"
    rm -f "$(PROGRAM)" "$(SYMBOLS)" "$(PROGRAM).tmp" "$(SYMBOLS).tmp"

-include $(OBJECTS:.o=.d)
