# Install GIT to clone packages
apt install git

# Make our source dir where we'll do all of the source
# building
cd $HOME
mkdir source
cd source

# Clone binutils-gdb source
git clone git://sourceware.org/git/binutils-gdb.git
# Clone GCC source
git clone git://gcc.gnu.org/git/gcc.git

# Environment variables
export PREFIX="$HOME/opt/cross_compiler"
export TARGET=x86_64-elf # i686-elf
export PATH="$PREFIX/bin:$PATH"

# Build binutils
cd $HOME/source
mkdir binutils-build
cd binutils-build
../binutils-gdb/configure --target=$TARGET --with-sysroot --disable-nls --disable-werror
make
make install

# Build GCC
cd $HOME/source
mkdir gcc-build
cd gcc-build
../gcc/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx
make -j 20 all-gcc
make -j 20 all-target-libgcc
make -j 20 all-target-libstdc++-v3
make install-gcc
make install-target-libgcc
make install-target-libstdc++-v3

# Add build directory to profile
line="export PATH=\"$HOME/opt/cross_compiler/bin:$PATH\""
profile="~/.profile"
touch $profile
grep -qF --"$line" "$profile" || echo "$line" >> "$profile"