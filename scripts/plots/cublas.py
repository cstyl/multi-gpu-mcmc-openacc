import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/'
gpu_timing = maindir + 'single-gpu/timing.csv'
cublas_timing = maindir + 'cublas/timing.csv'

N_LABEL=('10k', '100k', '1M', '10M')
N=(10000, 100000, 1000000, 10000000)
DIMS=10

labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

cublas_data = np.genfromtxt(cublas_timing, delimiter=',', skip_header=1)
gpu_data = np.genfromtxt(gpu_timing, delimiter=',', skip_header=1)

# 2 is for mean, std, 3 is for the implementations
time_mcmc = np.zeros((2, 2, len(N)))
time_lhood = np.zeros((2, 2, len(N)))
time_mvmul = np.zeros((2, 2, len(N)))
time_red = np.zeros((2, 2, len(N)))

# Single GPU first
for nidx,n in enumerate(N):

    block_N = gpu_data[:,3]
    data_N = gpu_data[block_N==n,:]

    time_mcmc[0,0,nidx] = data_N[:,7].mean()
    time_mcmc[0,1,nidx] = data_N[:,7].std()

    time_lhood[0,0,nidx] = data_N[:,8].mean()
    time_lhood[0,1,nidx] = data_N[:,8].std()

    time_mvmul[0,0,nidx] = data_N[:,9].mean()
    time_mvmul[0,1,nidx] = data_N[:,9].std()

    time_red[0,0,nidx] = data_N[:,10].mean()
    time_red[0,1,nidx] = data_N[:,10].std()

for nidx,n in enumerate(N):

    block_N = cublas_data[:,3]
    data_N = cublas_data[block_N==n,:]

    time_mcmc[1,0,nidx] = data_N[:,7].mean()
    time_mcmc[1,1,nidx] = data_N[:,7].std()

    time_lhood[1,0,nidx] = data_N[:,8].mean()
    time_lhood[1,1,nidx] = data_N[:,8].std()

    time_mvmul[1,0,nidx] = data_N[:,9].mean()
    time_mvmul[1,1,nidx] = data_N[:,9].std()

    time_red[1,0,nidx] = data_N[:,10].mean()
    time_red[1,1,nidx] = data_N[:,10].std()

# CUBLAS Execution time

plt.figure()
plt.errorbar(N, time_mcmc[1,0,:], yerr=time_mcmc[1,1,:],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[1,0,:], yerr=time_lhood[1,1,:],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[1,0,:], yerr=time_mvmul[1,1,:],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[1,0,:], yerr=time_red[1,1,:],
             marker=markers[3], color=colours[3], label=labels[3])
plt.xlabel('Datapoints')
plt.ylabel('Execution Time (s)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'cublas/cublas_runtime.eps', format='eps', dpi=1000)
plt.close()


plt.figure()
plt.errorbar(N, time_mcmc[0,0,:]/time_mcmc[1,0,:], yerr=time_mcmc[1,1,:],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[0,0,:]/time_lhood[1,0,:], yerr=time_lhood[1,1,:],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[0,0,:]/time_mvmul[1,0,:], yerr=time_mvmul[1,1,:],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[0,0,:]/time_red[1,0,:], yerr=time_red[1,1,:],
             marker=markers[3], color=colours[3], label=labels[3])
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{GPU}}{T_{CU}}$' + ' (Times)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'cublas/cublas_gpu_speedup.eps', format='eps', dpi=1000)
plt.close()
