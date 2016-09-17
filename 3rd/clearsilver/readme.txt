# install gcc48
brew install --enable-cxx homebrew/versions/gcc48

# configure
./configure CC=gcc-4.8 --prefix=$HOME/opt/clearsilver/ --disable-wdb --disable-apache --disable-python --disable-perl --disable-java --disable-csharp --disable-ruby

# drop test_crc.cs test's unit
sed -i 's/test_crc.cs//g' src/cs/Makefile

# build & install
make
make install
