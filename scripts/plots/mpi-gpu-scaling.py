import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import math

maindir = '../../experiments/'
mpi_timing = maindir + 'mpi-multi-node/timing.csv'
gpu_timing = maindir + 'mpi-single-node/timing.csv'


# N_LABEL=('100k', '1M', '10M', '100M')
# N=(100000, 1000000, 10000000, 100000000)
N_LABEL=('100k', '1M', '10M')
N=(100000, 1000000, 10000000)
DIMS=10
NODES=(1, 2, 3, 4, 6, 8)
# NODES=(1, 2, 3, 4)


labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

mpi_data = np.genfromtxt(mpi_timing, delimiter=',', skip_header=1)
gpu_data = np.genfromtxt(gpu_timing, delimiter=',', skip_header=1)


# 2 is for mean, std
time_mcmc = np.zeros((2, len(N), len(NODES), 2))
time_lhood = np.zeros((2, len(N), len(NODES), 2))
time_mvmul = np.zeros((2, len(N), len(NODES), 2))
time_red = np.zeros((2, len(N), len(NODES), 2))

for nidx,n in enumerate(N):

    block_N = mpi_data[:,3]
    data_N = mpi_data[block_N==n,:]

    for cidx,c in enumerate(NODES):

        block_core = data_N[:,0]
        data_core = data_N[block_core==c,:]

        time_mcmc[0, nidx,cidx,0] = data_core[:,7].mean() - data_core[:,14].mean() + data_core[:,11].mean() + data_core[:,10].mean()
        time_mcmc[0, nidx,cidx,1] = data_core[:,7].std() - data_core[:,14].std() + data_core[:,11].std() + data_core[:,10].std()

        time_lhood[0, nidx,cidx,0] = data_core[:,8].mean()
        time_lhood[0, nidx,cidx,1] = data_core[:,8].std()

        time_mvmul[0, nidx,cidx,0] = data_core[:,9].mean()
        time_mvmul[0, nidx,cidx,1] = data_core[:,9].std()

        time_red[0, nidx,cidx,0] = data_core[:,10].mean()
        time_red[0, nidx,cidx,1] = data_core[:,10].std()

for nidx,n in enumerate(N):

    block_N = gpu_data[:,3]
    data_N = gpu_data[block_N==n,:]

    for cidx,c in enumerate(NODES):

        block_core = data_N[:,1]
        data_core = data_N[block_core==c,:]

        time_mcmc[1, nidx,cidx,0] = data_core[:,7].mean() - data_core[:,14].mean() + data_core[:,11].mean() + data_core[:,10].mean()
        time_mcmc[1, nidx,cidx,1] = data_core[:,7].std() - data_core[:,14].std() + data_core[:,11].std() + data_core[:,10].std()

        time_lhood[1, nidx,cidx,0] = data_core[:,8].mean()
        time_lhood[1, nidx,cidx,1] = data_core[:,8].std()

        time_mvmul[1, nidx,cidx,0] = data_core[:,9].mean()
        time_mvmul[1, nidx,cidx,1] = data_core[:,9].std()

        time_red[1, nidx,cidx,0] = data_core[:,10].mean()
        time_red[1, nidx,cidx,1] = data_core[:,10].std()

    # plt.errorbar(NODES, time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
    #              marker=markers[1], color=colours[1], label=labels[1])
    #
    # plt.errorbar(NODES, time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
    #              marker=markers[2], color=colours[2], label=labels[2])
    #
    # plt.errorbar(NODES, time_red[nidx,:,0], yerr=time_red[nidx,:,1],
    #              marker=markers[3], color=colours[3], label=labels[3])

ax = plt.figure().gca()
plt.plot(NODES, time_mcmc[0,0,:,0]/time_mcmc[1,0,:,0], marker=markers[0], color=colours[0], label=N_LABEL[0])
plt.plot(NODES, time_mcmc[0,1,:,0]/time_mcmc[1,1,:,0], marker=markers[1], color=colours[1], label=N_LABEL[1])
plt.plot(NODES, time_mcmc[0,2,:,0]/time_mcmc[1,2,:,0], marker=markers[2], color=colours[2], label=N_LABEL[2])
# plt.plot(NODES, time_mcmc[0,3,:,0]/time_mcmc[1,3,:,0], marker=markers[3], color=colours[3], label=N_LABEL[3])
plt.xlabel('Processing Units')
plt.ylabel('Speed Up (Times)')
ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
# plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'mcmc_scaling_speedup.eps', format='eps', dpi=1000)
plt.close()

ax = plt.figure().gca()
plt.plot(NODES, time_lhood[0,0,:,0]/time_lhood[1,0,:,0], marker=markers[0], color=colours[0], label=N_LABEL[0])
plt.plot(NODES, time_lhood[0,1,:,0]/time_lhood[1,1,:,0], marker=markers[1], color=colours[1], label=N_LABEL[1])
plt.plot(NODES, time_lhood[0,2,:,0]/time_lhood[1,2,:,0], marker=markers[2], color=colours[2], label=N_LABEL[2])
# plt.plot(NODES, time_lhood[0,3,:,0]/time_lhood[1,3,:,0], marker=markers[3], color=colours[3], label=N_LABEL[3])
plt.xlabel('Processing Units')
plt.ylabel('Speed Up (Times)')
ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
# plt.yscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'lhood_scaling_speedup.eps', format='eps', dpi=1000)
plt.close()

ax = plt.figure().gca()
plt.plot(NODES, time_mvmul[0,0,:,0]/time_mvmul[1,0,:,0], marker=markers[0], color=colours[0], label=N_LABEL[0])
plt.plot(NODES, time_mvmul[0,1,:,0]/time_mvmul[1,1,:,0], marker=markers[1], color=colours[1], label=N_LABEL[1])
plt.plot(NODES, time_mvmul[0,2,:,0]/time_mvmul[1,2,:,0], marker=markers[2], color=colours[2], label=N_LABEL[2])
# plt.plot(NODES, time_mvmul[0,3,:,0]/time_mvmul[1,3,:,0], marker=markers[3], color=colours[3], label=N_LABEL[3])
plt.xlabel('Processing Units')
plt.ylabel('Speed Up (Times)')
# plt.yscale('log')
ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'mvmul_scaling_speedup.eps', format='eps', dpi=1000)
plt.close()

ax = plt.figure().gca()
plt.plot(NODES, time_red[0,0,:,0]/time_red[1,0,:,0], marker=markers[0], color=colours[0], label=N_LABEL[0])
plt.plot(NODES, time_red[0,1,:,0]/time_red[1,1,:,0], marker=markers[1], color=colours[1], label=N_LABEL[1])
plt.plot(NODES, time_red[0,2,:,0]/time_red[1,2,:,0], marker=markers[2], color=colours[2], label=N_LABEL[2])
# plt.plot(NODES, time_red[0,3,:,0]/time_red[1,3,:,0], marker=markers[3], color=colours[3], label=N_LABEL[3])
plt.xlabel('Processing Units')
plt.ylabel('Speed Up (Times)')
# plt.yscale('log')
ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'red_scaling_speedup.eps', format='eps', dpi=1000)
plt.close()
    #
    #
    # plt.figure()
    # plt.errorbar(NODES, time_mcmc[nidx,0,0]/time_mcmc[nidx,:,0], yerr=time_mcmc[nidx,:,1],
    #              marker=markers[0], color=colours[0], label=labels[0])
    #
    # plt.errorbar(NODES, time_lhood[nidx,0,0]/time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
    #              marker=markers[1], color=colours[1], label=labels[1])
    #
    # plt.errorbar(NODES, time_mvmul[nidx,0,0]/time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
    #              marker=markers[2], color=colours[2], label=labels[2])
    #
    # plt.errorbar(NODES, time_red[nidx,0,0]/time_red[nidx,:,0], yerr=time_red[nidx,:,1],
    #              marker=markers[3], color=colours[3], label=labels[3])
    # plt.plot(NODES, NODES, linestyle='--', color='black', label='ideal')
    # plt.xlabel('Nodes')
    # plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{n}}$' + ' (Times)')
    # # plt.yscale('log')
    # plt.legend()
    # plt.grid(True)
    # plt.savefig(maindir + 'mpi_scaling_speedup_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    # plt.close()
