##
## PROJECT:    MinLin
## FILE:       Makefile
## PURPOSE:    Makefile for MinLin.Tools solution
##
## LICENSE:    The MIT License
##
## MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
##

.RECIPEPREFIX := $(subst X, ,X)

.DEFAULT_GOAL := all
TOOLS_DIRECTORY := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# Add project directories here as they gain their own Makefiles.
PROJECTS ?= SynthRdp
CLEAN_PROJECTS := $(addprefix clean-,$(PROJECTS))

.PHONY: all clean $(PROJECTS) $(CLEAN_PROJECTS)
all: $(PROJECTS)

# Recursive make retains command-line overrides and the parallel jobserver.
$(PROJECTS):
    +$(MAKE) -C "$(TOOLS_DIRECTORY)/$@" all

clean: $(CLEAN_PROJECTS)
$(CLEAN_PROJECTS): clean-%:
    +$(MAKE) -C "$(TOOLS_DIRECTORY)/$*" clean
