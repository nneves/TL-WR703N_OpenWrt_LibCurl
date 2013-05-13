if [[ "$PATH" == *"/home/nelson/work/openwrt/trunk/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin"* ]]; then
  echo cross compiler toolchain path is already in PATH var
else
  export PATH="/home/nelson/work/openwrt/trunk/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2/bin:$PATH"
  echo added cross compiler toolchain path to PATH var
fi
 
#export AR=mips-openwrt-linux-ar
#export CC='mips-openwrt-linux-gcc -S'
#export CXX='mips-openwrt-linux-g++ -S'
#export LINK='mips-openwrt-linux-g++ -S'
#export RANLIB=mips-openwrt-linux-ranlib
#export CFLAGS='-march=mips32'
#export CCFLAGS='-march=mips32'
#export CXXFLAGS='-march=mips32'

#export LDFLAGS="-static"
#export CFLAGS="-Os -s -mips32 -mtune=mips32 -funit-at-a-time"
#export GYP_DEFINES="-Dtarget_arch=mips"
 

export PREFIX=/home/nelson/work/openwrt/trunk/staging_dir/toolchain-mips_r2_gcc-4.6-linaro_uClibc-0.9.33.2
export AR=$PREFIX/usr/bin/mips-openwrt-linux-uclibc-ar
export AS=$PREFIX/usr/bin/mips-openwrt-linux-uclibc-as
export LD=$PREFIX/usr/bin/mips-openwrt-linux-uclibc-ld
export NM=$PREFIX/usr/bin/mips-openwrt-linux-uclibc-nm
export CC=mips-openwrt-linux-uclibc-gcc
export CPP=mips-openwrt-linux-uclibc-cpp
export GCC=mips-openwrt-linux-uclibc-gcc
export CXX=mips-openwrt-linux-uclibc-g++
export RANLIB=mips-openwrt-linux-uclibc-ranlib
export ac_cv_linux_vers=2.4.30
export LDFLAGS=”-static”
export CFLAGS=”-Os -s”

make clean
./configure -host=mips-linux -prefix=$PREFIX/
make
