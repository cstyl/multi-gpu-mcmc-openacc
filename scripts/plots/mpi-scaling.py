import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/mpi-multi-node/'
timing = maindir + 'timing.csv'

N_LABEL=('10k', '100k', '1M', '10M')
N=(10000, 100000, 1000000, 10000000)
DIMS=10
NODES=(1, 2, 3, 4, 6, 8)


labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

data = np.genfromtxt(timing, delimiter=',', skip_header=1)

# 2 is for mean, std
time_mcmc = np.zeros((len(N), len(NODES), 2))
time_lhood = np.zeros((len(N), len(NODES), 2))
time_mvmul = np.zeros((len(N), len(NODES), 2))
time_red = np.zeros((len(N), len(NODES), 2))

for nidx,n in enumerate(N):

    block_N = data[:,3]
    data_N = data[block_N==n,:]

    for cidx,c in enumerate(NODES):

        block_core = data_N[:,0]
        data_core = data_N[block_core==c,:]

        time_mcmc[nidx,cidx,0] = data_core[:,7].mean()
        time_mcmc[nidx,cidx,1] = data_core[:,7].std()

        time_lhood[nidx,cidx,0] = data_core[:,8].mean()
        time_lhood[nidx,cidx,1] = data_core[:,8].std()

        time_mvmul[nidx,cidx,0] = data_core[:,9].mean()
        time_mvmul[nidx,cidx,1] = data_core[:,9].std()

        time_red[nidx,cidx,0] = data_core[:,10].mean()
        time_red[nidx,cidx,1] = data_core[:,10].std()


    plt.figure()
    plt.errorbar(NODES, time_mcmc[nidx,:,0], yerr=time_mcmc[nidx,:,1],
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(NODES, time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(NODES, time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(NODES, time_red[nidx,:,0], yerr=time_red[nidx,:,1],
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.xlabel('Nodes')
    plt.ylabel('Execution Time (s)')
    # plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(maindir + 'mpi_scaling_runtime_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()


    plt.figure()
    plt.errorbar(NODES, time_mcmc[nidx,0,0]/time_mcmc[nidx,:,0], yerr=time_mcmc[nidx,:,1],
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(NODES, time_lhood[nidx,0,0]/time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(NODES, time_mvmul[nidx,0,0]/time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(NODES, time_red[nidx,0,0]/time_red[nidx,:,0], yerr=time_red[nidx,:,1],
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.plot(NODES, NODES, linestyle='--', color='black', label='ideal')
    plt.xlabel('Nodes')
    plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{n}}$' + ' (Times)')
    # plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(maindir + 'mpi_scaling_speedup_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()
