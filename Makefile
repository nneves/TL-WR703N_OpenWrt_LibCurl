#----------------------------------------------------
# Generic Makefile for compiling a simple executable.
#----------------------------------------------------
LSRCDIR := libs
SRCDIR := src
BUILDDIR := build
SRCEXT := c
TARGET := simpledemo

#----------------------------------------------------
# OpenWrt cross-compile
#BASEDIR := /home/nelson/work/openwrt/trunk
#STAGING_DIR := $(BASEDIR)/staging_dir/
#XPATH := $(BASEDIR)/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin
#AR := $(XPATH)/mips-openwrt-linux-ar
#CC := $(XPATH)/mips-openwrt-linux-gcc
#CXX := $(XPATH)/mips-openwrt-linux-g++
#LINK := $(XPATH)/mips-openwrt-linux-g++
#RANLIB := $(XPATH)/mips-openwrt-linux-ranlib

#CFLAGS := '-march=mips32 -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE'
#CCFLAGS := '-march=mips32 -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE'
#CXXFLAGS := '-march=mips32 -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=c++0x'

#LDFLAGS := "-static"
#CDEFS := -DNDEBUG

#STATICLINKLIBS := -lcurl

#----------------------------------------------------
# Linux Standard
CC := gcc
CFLAGS := -g -Wall -pedantic -std=c++0x
STATICLINKLIBS := -lcurl

#----------------------------------------------------

LSOURCES := $(shell find $(LSRCDIR) -type f -name *.$(SRCEXT))
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

LOBJECTS := $(patsubst $(LSRCDIR)/%,$(BUILDDIR)/%,$(LSOURCES:.$(SRCEXT)=.o))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
 
$(TARGET): $(LOBJECTS) $(OBJECTS)
	@echo " Linking...$(TARGET)"; $(CC) $^ -o $(TARGET) $(STATICLINKLIBS)

# $< = libs/File.cpp
# $@ = libs/File.o
$(BUILDDIR)/%.o: $(LSRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) -I $(LSRCDIR) $(CFLAGS) -c $< -o $@

# $< = src/File.cpp
# $@ = src/File.o
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) -I $(LSRCDIR) $(CFLAGS) -c $< -o $@

clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)
 
.PHONY: clean