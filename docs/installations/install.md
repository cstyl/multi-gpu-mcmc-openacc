# Pre-requisites

### Argtable C command line parser:

To download, build and install see [Argtable](http://argtable.sourceforge.net). The following provides a quick summary of the key steps to build and install Argtable (version 2.13) in your home directory:
```sh
$ wget http://prdownloads.sourceforge.net/argtable/argtable2-13.tar.gz
$ tar -xvzf argtable2-13.tar.gz
$ cd argtable2-13
$ ./configure --prefix=$HOME
$ make
$ make check (optional)
$ make install
$ ls $HOME/include/argtable*
argtable2.h
$ ls $HOME/lib/libargtable*
libargtable2.a  libargtable2.la  libargtable2.so  libargtable2.so.0  libargtable2.so.0.1.8
$ ls $HOME/share/man $HOME/share/doc/
doc/:
argtable2

man:
man1  man3
$ make clean
$ export C_INCLUDE_PATH=$HOME/include:$C_INCLUDE_PATH
$ export LIBRARY_PATH=$HOME/lib:$LIBRARY_PATH
$ export LD_LIBRARY_PATH=$HOME/lib:$LD_LIBRARY_PATH
$ cd ../
$ rm -rf argtable2-13*
```
### CUnit test framework:

To download, build and install see [CUnit](http://cunit.sourceforge.net/). The following provides a quick summary of the key steps to build and install CUnit (version 2.1.2) in your home directory:

```sh
$ wget http://downloads.sourceforge.net/project/cunit/CUnit/2.1-2/CUnit-2.1-2-src.tar.bz2
$ tar -xvjpf CUnit-2.1-2-src.tar.bz2
$ cd CUnit-2.1-2
$ ./configure --prefix=$HOME
$ make
$ make install
$ ls $HOME/include/CUnit
Util.h     TestDB.h  CUnit_intl.h  CUError.h  Basic.h
TestRun.h  MyMem.h   CUnit.h       Console.h  Automated.h
$ ls $HOME/lib
libcunit.a   libcunit.so    libcunit.so.1.0.1
libcunit.la  libcunit.so.1
$ ls $HOME/share/CUnit
CUnit-List.dtd  CUnit-Run.dtd  Memory-Dump.dtd
CUnit-List.xsl  CUnit-Run.xsl  Memory-Dump.xsl
$ export C_INCLUDE_PATH=$HOME/include:$C_INCLUDE_PATH
$ export LIBRARY_PATH=$HOME/lib:$LIBRARY_PATH
$ export LD_LIBRARY_PATH=$HOME/lib:$LD_LIBRARY_PATH
$ cd ../
$ rm -rf CUnit*
```

### GSL - GNU Scientific Library:

To download, build and install see [GSL](https://www.gnu.org/software/gsl/). The following provides a quick summary of the key steps to build and install GSL (version 2.5) in your home directory:
```sh
$ wget http://ftp.snt.utwente.nl/pub/software/gnu/gsl/gsl-latest.tar.gz
$ ls -d gsl*
gsl-latest.tar.gz
$ tar -xvzf gsl-latest.tar.gz
$ ls -d gsl*
gsl-2.5  gsl-latest.tar.gz
$ cd gsl-2.5
$ ./configure --prefix=$HOME
$ make
$ make check (optional)
$ make install
$ ls $HOME/include/gsl
gsl_blas.h                       gsl_ieee_utils.h                   gsl_permute_float.h                       gsl_sf_clausen.h               gsl_sort_vector_long.h
gsl_blas_types.h                 gsl_inline.h                       gsl_permute.h                             gsl_sf_coulomb.h               gsl_sort_vector_short.h
...
$ ls $HOME/lib/libgsl*
libgsl.a        libgslcblas.so        libgsl.la     libgsl.so.23.1.0
libgslcblas.a   libgslcblas.so.0      libgsl.so
libgslcblas.la  libgslcblas.so.0.0.0  libgsl.so.23
$ ls $HOME/share/aclocal $HOME/share/info/
aclocal:
gsl.m4

info/:
gsl-ref.info
$ make clean
$ export C_INCLUDE_PATH=$HOME/include:$C_INCLUDE_PATH
$ export LIBRARY_PATH=$HOME/lib:$LIBRARY_PATH
$ export LD_LIBRARY_PATH=$HOME/lib:$LD_LIBRARY_PATH
$ cd ../
$ rm -rf gsl*
```

### HDF5:
To download, build and install see [HDF5](https://www.hdfgroup.org/downloads/hdf5/source-code/#). The following provides a quick summary of the key steps to build and install HDF5 (version 1.10.4) in your home directory:

```sh
$ wget https://s3.amazonaws.com/hdf-wordpress-1/wp-content/uploads/manual/HDF5/HDF5_1_10_4/hdf5-1.10.4.tar.bz2
$ tar -xvjpf hdf5-1.10.4.tar.bz2
$ cd hdf5-1.10.4
$ ./configure --prefix=$HOME
$ make
$ make install
$ export C_INCLUDE_PATH=$HOME/include:$C_INCLUDE_PATH
$ export LIBRARY_PATH=$HOME/lib:$LIBRARY_PATH
$ export LD_LIBRARY_PATH=$HOME/lib:$LD_LIBRARY_PATH
$ cd ../
$ rm -rf hdf5*
```
