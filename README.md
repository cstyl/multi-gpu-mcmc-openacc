# Multi-GPU MCMC Implementation
Implementation of Metropolis-MCMC using multi-GPUs approach on OpenACC as part of the MSc HPC with Data Science Dissertation in EPCC.


## Requirements:

- OpenBLAS
To download and install:
```sh
$  git clone https://github.com/xianyi/OpenBLAS.git
$  cd OpenBLAS
$  make USE_THREAD=0 USE_OPENMP=0
$  make install PREFIX=your_installation_directory
```


### Build and Run on CIRRUS:
Copy the config file in root dir:
```sh
$ cp config/linux-pgi-default.mk ./config.mk
```
Remember to specify the paths in the files

Load PGI compilers:
```sh
$ module load pgi
```

For serial and threaded versions you will need to build mpi_s:
```sh
$ cd mpi_s
$ make clean
$ make libc
```
For MPI Runs you will need mpt:
```sh
$ module load mpt
```

For GPU Runs you will need cuda:
```sh
$ module load cuda
```

To compile the code:
```sh
$ cd src
$ make verbose
```
This will display the options and select one of the options.

To run serial or threaded executable:
```sh
$ cd src
$ ./mcmc.exe input.ref
```
This will run the executable with the contents of input.ref. You can change the threads from there.

To run mpi executable submit in a pbs:
```sh
$ cd src
$ mpiexec_mpt -ppn N -n M ./mcmc.exe input.ref
```
or for hybrid
```sh
$ cd src
$ mpiexec_mpt -ppn N -n M omplace -nt K ./mcmc.exe input.ref
```
Remember to load the necessary modules.

To run on GPUs compile one of the gpu options and submit on the gpu node
