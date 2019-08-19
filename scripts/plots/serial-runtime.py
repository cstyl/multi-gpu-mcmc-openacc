import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/serial-ref/'
timing = maindir + 'timing.csv'

N=(10000, 100000, 1000000, 10000000)

labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

data = np.genfromtxt(timing, delimiter=',', skip_header=1)

# 2 is for mean, std
time_mcmc = np.zeros((len(N), 2))
time_lhood = np.zeros((len(N), 2))
time_mvmul = np.zeros((len(N), 2))
time_red = np.zeros((len(N), 2))

for nidx,n in enumerate(N):

    block_N = data[:,3]
    data_N = data[block_N==n,:]

    time_mcmc[nidx,0] = data_N[:,7].mean()
    time_mcmc[nidx,1] = data_N[:,7].std()

    time_lhood[nidx,0] = data_N[:,8].mean()
    time_lhood[nidx,1] = data_N[:,8].std()

    time_mvmul[nidx,0] = data_N[:,9].mean()
    time_mvmul[nidx,1] = data_N[:,9].std()

    time_red[nidx,0] = data_N[:,10].mean()
    time_red[nidx,1] = data_N[:,10].std()


plt.figure()
plt.errorbar(N, time_mcmc[:,0], yerr=time_mcmc[:,1],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[:,0], yerr=time_lhood[:,1],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mvmul[:,0], yerr=time_mvmul[:,1],
             marker=markers[2], color=colours[2], label=labels[2])

plt.errorbar(N, time_red[:,0], yerr=time_red[:,1],
             marker=markers[3], color=colours[3], label=labels[3])

plt.xlabel('Datapoints N')
plt.ylabel('Execution Time (s)')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'serial_runtime.eps', format='eps', dpi=1000)
plt.close()
