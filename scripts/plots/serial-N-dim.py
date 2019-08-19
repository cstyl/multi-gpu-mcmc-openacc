import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/serial-perf/'
timing = maindir + 'timing.csv'

N=(1000, 10000, 100000, 1000000)
DIMS=(3, 10, 50)

labels = ('d=3', 'd=10', 'd=50')
colours = ('dodgerblue', 'mediumseagreen', 'orange')
markers = ('o', 'v', '1')

data = np.genfromtxt(timing, delimiter=',', skip_header=1)

# 2 is for mean, std
time_total = np.zeros((len(DIMS), len(N), 2))
time_mcmc = np.zeros((len(DIMS), len(N), 2))
time_lhood = np.zeros((len(DIMS), len(N), 2))


for didx,dim in enumerate(DIMS):

    block_dim = data[:,4]
    data_dim = data[block_dim==dim,:]

    for nidx,n in enumerate(N):

        block_N = data_dim[:,3]
        data_N = data_dim[block_N==n,:]

        time_total[didx,nidx,0] = data_N[:,6].mean()
        time_total[didx,nidx,1] = data_N[:,6].std()

        time_mcmc[didx,nidx,0] = data_N[:,7].mean()
        time_mcmc[didx,nidx,1] = data_N[:,7].std()

        time_lhood[didx,nidx,0] = data_N[:,8].mean()
        time_lhood[didx,nidx,1] = data_N[:,8].std()


plt.figure()
plt.errorbar(N, time_total[0,:,0], yerr=time_total[0,:,1],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_total[1,:,0], yerr=time_total[1,:,1],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_total[2,:,0], yerr=time_total[2,:,1],
             marker=markers[2], color=colours[2], label=labels[2])
plt.xlabel('Datapoints N')
plt.ylabel('Execution Time (s)')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'total_time.eps', format='eps', dpi=1000)
plt.close()



plt.figure()
plt.errorbar(N, time_mcmc[0,:,0], yerr=time_mcmc[0,:,1],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_mcmc[1,:,0], yerr=time_mcmc[1,:,1],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_mcmc[2,:,0], yerr=time_mcmc[2,:,1],
             marker=markers[2], color=colours[2], label=labels[2])
plt.xlabel('Datapoints N')
plt.ylabel('Execution Time (s)')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'mcmc_time.eps', format='eps', dpi=1000)
plt.close()



plt.figure()
plt.errorbar(N, time_lhood[0,:,0], yerr=time_lhood[0,:,1],
             marker=markers[0], color=colours[0], label=labels[0])

plt.errorbar(N, time_lhood[1,:,0], yerr=time_lhood[1,:,1],
             marker=markers[1], color=colours[1], label=labels[1])

plt.errorbar(N, time_lhood[2,:,0], yerr=time_lhood[2,:,1],
             marker=markers[2], color=colours[2], label=labels[2])
plt.xlabel('Datapoints N')
plt.ylabel('Execution Time (s)')
plt.xscale('log')
plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'lhood_time.eps', format='eps', dpi=1000)
plt.close()
