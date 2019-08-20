import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/'
omp_timing = maindir + 'omp/timing.csv'
mpi_timing = maindir + 'mpi/timing.csv'
gpu_timing = maindir + 'single-gpu/timing.csv'

N_LABEL=('10k', '100k', '1M', '10M')
N=(10000, 100000, 1000000, 10000000)
DIMS=10

labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

omp_data = np.genfromtxt(omp_timing, delimiter=',', skip_header=1)
mpi_data = np.genfromtxt(mpi_timing, delimiter=',', skip_header=1)
gpu_data = np.genfromtxt(gpu_timing, delimiter=',', skip_header=1)

# 2 is for mean, std, 3 is for the implementations
time_mcmc = np.zeros((3, 2, len(N)))
time_lhood = np.zeros((3, 2, len(N)))
time_mvmul = np.zeros((3, 2, len(N)))
time_red = np.zeros((3, 2, len(N)))

# OMP data first (need only the 36 cores)
for nidx,n in enumerate(N):

    block_N = omp_data[:,3]
    data_N = omp_data[block_N==n,:]

    block_core = data_N[:,2]
    data_core = data_N[block_core==36,:]

    time_mcmc[0,0,nidx] = data_core[:,7].mean()
    time_mcmc[0,1,nidx] = data_core[:,7].std()

    time_lhood[0,0,nidx] = data_core[:,8].mean()
    time_lhood[0,1,nidx] = data_core[:,8].std()

    time_mvmul[0,0,nidx] = data_core[:,9].mean()
    time_mvmul[0,1,nidx] = data_core[:,9].std()

    time_red[0,0,nidx] = data_core[:,10].mean()
    time_red[0,1,nidx] = data_core[:,10].std()

# MPI data (need only the 36 cores)
for nidx,n in enumerate(N):

    block_N = mpi_data[:,3]
    data_N = mpi_data[block_N==n,:]

    block_core = data_N[:,1]
    data_core = data_N[block_core==36,:]

    time_mcmc[1,0,nidx] = data_core[:,7].mean()
    time_mcmc[1,1,nidx] = data_core[:,7].std()

    time_lhood[1,0,nidx] = data_core[:,8].mean()
    time_lhood[1,1,nidx] = data_core[:,8].std()

    time_mvmul[1,0,nidx] = data_core[:,9].mean()
    time_mvmul[1,1,nidx] = data_core[:,9].std()

    time_red[1,0,nidx] = data_core[:,10].mean()
    time_red[1,1,nidx] = data_core[:,10].std()

# Now for gpu (core=1)
for nidx,n in enumerate(N):

    block_N = gpu_data[:,3]
    data_N = gpu_data[block_N==n,:]

    block_core = data_N[:,2]
    data_core = data_N[block_core==1,:]

    time_mcmc[2,0,nidx] = data_core[:,7].mean()
    time_mcmc[2,1,nidx] = data_core[:,7].std()

    time_lhood[2,0,nidx] = data_core[:,8].mean()
    time_lhood[2,1,nidx] = data_core[:,8].std()

    time_mvmul[2,0,nidx] = data_core[:,9].mean()
    time_mvmul[2,1,nidx] = data_core[:,9].std()

    time_red[2,0,nidx] = data_core[:,10].mean()
    time_red[2,1,nidx] = data_core[:,10].std()

# GPU Execution time

plt.figure()
plt.errorbar(N, time_mcmc[2,0,:], yerr=time_mcmc[2,1,:],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[2,0,:], yerr=time_lhood[2,1,:],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[2,0,:], yerr=time_mvmul[2,1,:],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[2,0,:], yerr=time_red[2,1,:],
             marker=markers[3], color=colours[3], label=labels[3])
plt.xlabel('Datapoints')
plt.ylabel('Execution Time (s)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'single-gpu/single_gpu_runtime.eps', format='eps', dpi=1000)
plt.close()


plt.figure()
plt.errorbar(N, time_mcmc[0,0,:]/time_mcmc[2,0,:], yerr=time_mcmc[2,1,:],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[0,0,:]/time_lhood[2,0,:], yerr=time_lhood[2,1,:],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[0,0,:]/time_mvmul[2,0,:], yerr=time_mvmul[2,1,:],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[0,0,:]/time_red[2,0,:], yerr=time_red[2,1,:],
             marker=markers[3], color=colours[3], label=labels[3])
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{OMP}}{T_{GPU}}$' + ' (Times)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'single-gpu/gpu_omp_speedup.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
plt.errorbar(N, time_mcmc[1,0,:]/time_mcmc[2,0,:], yerr=time_mcmc[2,1,:],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[1,0,:]/time_lhood[2,0,:], yerr=time_lhood[2,1,:],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[1,0,:]/time_mvmul[2,0,:], yerr=time_mvmul[2,1,:],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[1,0,:]/time_red[2,0,:], yerr=time_red[2,1,:],
             marker=markers[3], color=colours[3], label=labels[3])
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{MPI}}{T_{GPU}}$' + ' (Times)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'single-gpu/gpu_mpi_speedup.eps', format='eps', dpi=1000)
plt.close()
