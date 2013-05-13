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
export BASEDIR := /home/nelson/work/openwrt/trunk
export STAGING_DIR := $(BASEDIR)/staging_dir/
export XPATH := $(BASEDIR)/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin
AR := $(XPATH)/mips-openwrt-linux-ar
CC := $(XPATH)/mips-openwrt-linux-gcc
CXX := $(XPATH)/mips-openwrt-linux-g++
LINK := $(XPATH)/mips-openwrt-linux-g++
RANLIB := $(XPATH)/mips-openwrt-linux-ranlib

CFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=gnu99
CCFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE
CXXFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=c++0x

LDFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE 
#-static
CDEFS := -DNDEBUG

STATICLINKLIBS := -ldl -lcurl -lz -lssl -lcrypto
STATICLINKPATH := libs/library/
INCLUDEDIR := libs/curl

#----------------------------------------------------
# Linux Standard
#CC := gcc

#CFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE
#CCFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE
#CXXFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=c++0x

#LDFLAGS := -g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE 
#-static
#CDEFS := -DNDEBUG

#STATICLINKLIBS := -ldl -lcurl -lz -lssl -lcrypto
#STATICLINKPATH := libs/library/
#INCLUDEDIR := /usr/include/

#----------------------------------------------------

LSOURCES := $(shell find $(LSRCDIR) -type f -name *.$(SRCEXT))
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

LOBJECTS := $(patsubst $(LSRCDIR)/%,$(BUILDDIR)/%,$(LSOURCES:.$(SRCEXT)=.o))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
 
$(TARGET): $(LOBJECTS) $(OBJECTS)
	@echo " Linking...$(TARGET)"; $(CC) $^ -o $(TARGET) $(LDFLAGS) -L $(STATICLINKPATH) $(STATICLINKLIBS)

# $< = libs/File.cpp
# $@ = libs/File.o
$(BUILDDIR)/%.o: $(LSRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) -I $(LSRCDIR) -I $(INCLUDEDIR) $(CFLAGS) -c $< -o $@

# $< = src/File.cpp
# $@ = src/File.o
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CC $<"; $(CC) -I $(LSRCDIR) -I $(INCLUDEDIR) $(CFLAGS) -c $< -o $@

deploy:
	@scp ./$(TARGET) root@192.168.1.250:/mnt/usbstorage/;
clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)
 
.PHONY: clean