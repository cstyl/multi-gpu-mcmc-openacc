import numpy as np
import matplotlib.pyplot as plt
import math

def speed_up_error(T1, Tp, dT1, dTp):

    return np.sqrt(np.power(dT1/Tp,2) + np.power(T1*dTp/np.power(Tp,2),2))

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
time_mvmul = np.zeros((2, 2, len(N)))

# Single GPU first
for nidx,n in enumerate(N):

    block_N = gpu_data[:,3]
    data_N = gpu_data[block_N==n,:]

    time_mvmul[0,0,nidx] = data_N[:,9].mean()
    time_mvmul[0,1,nidx] = data_N[:,9].std()

for nidx,n in enumerate(N):

    block_N = cublas_data[:,3]
    data_N = cublas_data[block_N==n,:]

    time_mvmul[1,0,nidx] = data_N[:,9].mean()
    time_mvmul[1,1,nidx] = data_N[:,9].std()

error = speed_up_error(time_mvmul[0,0], time_mvmul[1,0], time_mvmul[0,1], time_mvmul[1,1])

# CUBLAS Execution time

plt.figure()
plt.errorbar(N, time_mvmul[1,0,:], yerr=time_mvmul[1,1,:],
             marker=markers[2], color=colours[2], label=labels[2])
plt.xlabel('Datapoints')
plt.ylabel('Execution Time (s)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'cublas/cublas_runtime.eps', format='eps', dpi=1000)
plt.close()


plt.figure()
plt.errorbar(N, time_mvmul[0,0,:]/time_mvmul[1,0,:], yerr=error,
             marker=markers[2], color=colours[2], label=labels[2])
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{GPU}}{T_{CU}}$' + ' (Times)')
plt.yscale('log')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'cublas/cublas_gpu_speedup.eps', format='eps', dpi=1000)
plt.close()
