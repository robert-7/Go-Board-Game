DevIL (A full featured cross-platform image library)

For convenience, I pre-built the binaries for DevIL on CDF using the code that's freely available on openil.sourceforge.net.  The binaries in these subdirectories provide an easy way to load in images (.jpg, .tif, .bmp, etc) into graphics memory.

To compile the code yourself, follow the following steps:
1.  Download the code, http://downloads.sourceforge.net/openil/DevIL-1.7.8.tar.gz
2.  Unzip the file, "tar xzvf DevIL-1.7.8.tar.gz".
3.  Type "autoreconf -i"
4.  Type "./configure --enable-ILU --enable-ILUT --prefix=<installation directory>/usr"
5.  Type "make"
6.  Type "make install"
More information about these steps can be found in the README.unix document in the DevIL-1.7.8.tar.gz file.