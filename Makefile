#----------------------------------------------------
# Generic Makefile for compiling a simple executable.
#----------------------------------------------------
LSRCDIR=libs
SRCDIR=src
BUILDDIR=build
#SRCEXT=c
SRCEXT=cpp
TARGET=cloudprintercli

#----------------------------------------------------
# OpenWrt cross-compile
export BASEDIR=/home/nelson/work/openwrt/trunk
export STAGING_DIR=$(BASEDIR)/staging_dir/
export TOOLCHAIN=$(BASEDIR)/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin
export AR=$(TOOLCHAIN)/mips-openwrt-linux-ar
#export CC=$(TOOLCHAIN)/mips-openwrt-linux-gcc
#export CXX=$(TOOLCHAIN)/mips-openwrt-linux-g++
#export LINK=$(TOOLCHAIN)/mips-openwrt-linux-g++
#export RANLIB=$(TOOLCHAIN)/mips-openwrt-linux-ranlib

export LD=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-ld 
export NM=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-nm 
export CC=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-gcc
export GCC=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-gcc
export CXX=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-g++
export RANLIB=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-ranlib 
export STRIP=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-strip
export OBJCOPY=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-objcopy
export OBJDUMP=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-objdump 
export SIZE=$(TOOLCHAIN)/mips-openwrt-linux-uclibc-size 
#export CFLAGS=-std=gnu99 -Os -pipe -mips32 -mtune=mips32 -funit-at-a-time -fhonour-copts -msoft-float -DARGTYPE=3 -ffunction-sections -fdata-sections 
#export CXXFLAGS=-std=c++0x -Os -pipe -mips32 -mtune=mips32 -funit-at-a-time -fhonour-copts -msoft-float -DARGTYPE=3 -ffunction-sections -fdata-sections 
#export CPPFLAGS=-I${TOOLCHAIN}/../usr/include -I${TOOLCHAIN}/../include 
#export LDFLAGS=-L${TOOLCHAIN}/../usr/lib -L${TOOLCHAIN}/../lib -Wl,--gc-sections -Wl,-rpath,${TOOLCHAIN}/../lib -Wl,-s
#LDFLAGS += -Wl,-rpath-link,$(STAGING_DIR)/usr/lib:$(STAGING_DIR)/lib,-lcurl,-lxml2,-lz,-lssl


STATICLINKLIBS=-ldl -lcurl -lz -lssl -lcrypto -lpthread -lstdc++
STATICLINKPATH=libs/library
INCLUDEDIR=libs/curl

export CFLAGS=-g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=gnu99
export CCFLAGS=-g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE
export CXXFLAGS=-g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE -std=c++0x
export LDFLAGS=-g -Wall -pedantic -DDEBUG -D_REENTRANT -D_BSD_SOURCE

#export CDEFS=-DNDEBUG

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

LSOURCES=$(shell find $(LSRCDIR) -type f -name *.$(SRCEXT))
SOURCES=$(shell find $(SRCDIR) -type f -name *.$(SRCEXT))

LOBJECTS=$(patsubst $(LSRCDIR)/%,$(BUILDDIR)/%,$(LSOURCES:.$(SRCEXT)=.o))
OBJECTS=$(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
 
$(TARGET): $(LOBJECTS) $(OBJECTS)
	@echo " Linking...$(TARGET)";
	$(CXX) $^ -o $(TARGET) $(LDFLAGS) -Wl,-rpath-link,libs/library/,-ldl,-lcurl,-lz,-lssl,-lcrypto,-lpthread,-lstdc++ -L $(STATICLINKPATH) $(STATICLINKLIBS) 

# $< = libs/File.cpp
# $@ = libs/File.o
$(BUILDDIR)/%.o: $(LSRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CXX $<"; $(CC) -I $(LSRCDIR) -I $(INCLUDEDIR) $(CXXFLAGS) -c $< -o $@

# $< = src/File.cpp
# $@ = src/File.o
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " CXX $<"; $(CC) -I $(LSRCDIR) -I $(INCLUDEDIR) $(CXXFLAGS) -c $< -o $@

deploy:
	@scp ./$(TARGET) root@192.168.1.250:/mnt/usbstorage/;
clean:
	@echo " Cleaning..."; $(RM) -r $(BUILDDIR) $(TARGET)
 
.PHONY: clean