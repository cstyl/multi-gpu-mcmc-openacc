import numpy as np
import matplotlib.pyplot as plt
import math
from matplotlib.ticker import MaxNLocator

def speed_up_error(T1, Tp, dT1, dTp):

    return np.sqrt(np.power(dT1/Tp,2) + np.power(T1*dTp/np.power(Tp,2),2))

maindir = '../../experiments/mpi-single-node/'
timing = maindir + 'timing.csv'

N_LABEL=('100k', '1M', '10M', '100M')
N=(100000, 1000000, 10000000, 100000000)
DIMS=10
CORES=(1, 2, 3, 4, 6, 8)


labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue', 'maroon', 'darkkhaki')
markers = ('o', 'v', '^', '1', '*', '+')
# colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
# markers = ('o', 'v', '1', '*')

data = np.genfromtxt(timing, delimiter=',', skip_header=1)

# 2 is for mean, std
time_mcmc = np.zeros((len(N), len(CORES), 2))
time_lhood = np.zeros((len(N), len(CORES), 2))
time_mvmul = np.zeros((len(N), len(CORES), 2))
time_red = np.zeros((len(N), len(CORES), 2))

for nidx,n in enumerate(N):

    block_N = data[:,3]
    data_N = data[block_N==n,:]

    for cidx,c in enumerate(CORES):

        block_core = data_N[:,1]
        data_core = data_N[block_core==c,:]

        time_mcmc[nidx,cidx,0] = data_core[:,7].mean() - data_core[:,14].mean() + data_core[:,11].mean() + data_core[:,10].mean()
        time_mcmc[nidx,cidx,1] = data_core[:,7].std() - data_core[:,14].std() + data_core[:,11].std() + data_core[:,10].std()

        time_lhood[nidx,cidx,0] = data_core[:,8].mean()
        time_lhood[nidx,cidx,1] = data_core[:,8].std()

        time_mvmul[nidx,cidx,0] = data_core[:,9].mean()
        time_mvmul[nidx,cidx,1] = data_core[:,9].std()

        time_red[nidx,cidx,0] = data_core[:,10].mean()
        time_red[nidx,cidx,1] = data_core[:,10].std()

    mcmc_error = speed_up_error(time_mcmc[nidx,0,0], time_mcmc[nidx,:,0],
                                time_mcmc[nidx,0,1], time_mcmc[nidx,:,1])

    lhood_error = speed_up_error(time_lhood[nidx,0,0], time_lhood[nidx,:,0],
                                time_lhood[nidx,0,1], time_lhood[nidx,:,1])

    mvmul_error = speed_up_error(time_mvmul[nidx,0,0], time_mvmul[nidx,:,0],
                                time_mvmul[nidx,0,1], time_mvmul[nidx,:,1])

    red_error = speed_up_error(time_red[nidx,0,0], time_red[nidx,:,0],
                                time_red[nidx,0,1], time_red[nidx,:,1])

    ax = plt.figure().gca()
    plt.errorbar(CORES, time_mcmc[nidx,:,0], yerr=time_mcmc[nidx,:,1],
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(CORES, time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(CORES, time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(CORES, time_red[nidx,:,0], yerr=time_red[nidx,:,1],
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.xlabel('GPUs')
    plt.ylabel('Execution Time (s)')
    ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
    # plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(maindir + 'mpi-single-node_runtime_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()


    ax = plt.figure().gca()
    plt.errorbar(CORES, time_mcmc[nidx,0,0]/time_mcmc[nidx,:,0], yerr=mcmc_error,
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(CORES, time_lhood[nidx,0,0]/time_lhood[nidx,:,0], yerr=lhood_error,
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(CORES, time_mvmul[nidx,0,0]/time_mvmul[nidx,:,0], yerr=mvmul_error,
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(CORES, time_red[nidx,0,0]/time_red[nidx,:,0], yerr=red_error,
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
    plt.xlabel('GPUs')
    plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
    # plt.yscale('log')
    plt.legend()
    ax.xaxis.set_major_locator(MaxNLocator(nbins=4,integer=True))
    plt.grid(True)
    plt.savefig(maindir + 'mpi-single-node_speedup_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()


# mcmc_error = np.zeros((len(N),6))
# lhood_error = np.zeros((len(N),6))
# mvmul_error = np.zeros((len(N),6))
# red_error = np.zeros((len(N),6))

# for i in range(5):
#     idx = CORES[i]
#     mcmc_error = speed_up_error(time_mcmc[:,0,0], time_mcmc[:,idx,0],
#                                 time_mcmc[:,0,1], time_mcmc[:,idx,1])
#     lhood_error = speed_up_error(time_lhood[:,0,0], time_lhood[:,idx,0],
#                                 time_lhood[:,0,1], time_lhood[:,idx,1])
#     mvmul_error = speed_up_error(time_mvmul[:,0,0], time_mvmul[:,idx,0],
#                                 time_mvmul[:,0,1], time_mvmul[:,idx,1])
#     red_error = speed_up_error(time_red[:,0,0], time_red[:,idx,0],
#                                 time_red[:,0,1], time_red[:,idx,1])

labels = ('GPUs=1', 'GPUs=2', 'GPUs=3', 'GPUs=4', 'GPUs=6', 'GPUs=8')

plt.figure()
# plt.errorbar(N, time_mcmc[:,0,0]/time_mcmc[:,0,0], yerr=mcmc_error,
#              marker=markers[1], color=colours[1], label=labels[0])
# plt.errorbar(N, time_mcmc[:,0,0]/time_mcmc[:,1,0], yerr=mcmc_error,
#              marker=markers[1], color=colours[1], label=labels[1])
# plt.errorbar(N, time_mcmc[:,2,0]/time_mcmc[:,2,0], yerr=mcmc_error,
#              marker=markers[1], color=colours[1], label=labels[2])
# plt.errorbar(N, time_mcmc[:,3,0]/time_mcmc[:,3,0], yerr=mcmc_error,
#              marker=markers[1], color=colours[1], label=labels[3])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,0,0], marker=markers[0], color=colours[0], label=labels[0])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,1,0], marker=markers[1], color=colours[1], label=labels[1])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,2,0], marker=markers[2], color=colours[2], label=labels[2])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,3,0], marker=markers[3], color=colours[3], label=labels[3])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,4,0], marker=markers[4], color=colours[4], label=labels[4])
plt.plot(N, time_mcmc[:,0,0]/time_mcmc[:,5,0], marker=markers[5], color=colours[5], label=labels[5])
# plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'mcmc_speedup.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
# plt.errorbar(N, time_lhood[:,0,0]/time_lhood[:,0,0], yerr=lhood_error,
#              marker=markers[1], color=colours[1], label=labels[0])
# plt.errorbar(N, time_lhood[:,0,0]/time_lhood[:,1,0], yerr=lhood_error,
#              marker=markers[1], color=colours[1], label=labels[1])
# plt.errorbar(N, time_lhood[:,2,0]/time_lhood[:,2,0], yerr=lhood_error,
#              marker=markers[1], color=colours[1], label=labels[2])
# plt.errorbar(N, time_lhood[:,3,0]/time_lhood[:,3,0], yerr=lhood_error,
#              marker=markers[1], color=colours[1], label=labels[3])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,0,0], marker=markers[0], color=colours[0], label=labels[0])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,1,0], marker=markers[1], color=colours[1], label=labels[1])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,2,0], marker=markers[2], color=colours[2], label=labels[2])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,3,0], marker=markers[3], color=colours[3], label=labels[3])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,4,0], marker=markers[4], color=colours[4], label=labels[4])
plt.plot(N, time_lhood[:,0,0]/time_lhood[:,5,0], marker=markers[5], color=colours[5], label=labels[5])
# plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'lhood_speedup.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
# plt.errorbar(N, time_mvmul[:,0,0]/time_mvmul[:,0,0], yerr=mvmul_error,
#              marker=markers[1], color=colours[1], label=labels[0])
# plt.errorbar(N, time_mvmul[:,0,0]/time_mvmul[:,1,0], yerr=mvmul_error,
#              marker=markers[1], color=colours[1], label=labels[1])
# plt.errorbar(N, time_mvmul[:,2,0]/time_mvmul[:,2,0], yerr=mvmul_error,
#              marker=markers[1], color=colours[1], label=labels[2])
# plt.errorbar(N, time_mvmul[:,3,0]/time_mvmul[:,3,0], yerr=mvmul_error,
#              marker=markers[1], color=colours[1], label=labels[3])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,0,0], marker=markers[0], color=colours[0], label=labels[0])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,1,0], marker=markers[1], color=colours[1], label=labels[1])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,2,0], marker=markers[2], color=colours[2], label=labels[2])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,3,0], marker=markers[3], color=colours[3], label=labels[3])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,4,0], marker=markers[4], color=colours[4], label=labels[4])
plt.plot(N, time_mvmul[:,0,0]/time_mvmul[:,5,0], marker=markers[5], color=colours[5], label=labels[5])
# plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'mvmul_speedup.eps', format='eps', dpi=1000)
plt.close()


plt.figure()
# plt.errorbar(N, time_red[:,0,0]/time_red[:,0,0], yerr=red_error,
#              marker=markers[1], color=colours[1], label=labels[0])
# plt.errorbar(N, time_red[:,0,0]/time_red[:,1,0], yerr=red_error,
#              marker=markers[1], color=colours[1], label=labels[1])
# plt.errorbar(N, time_red[:,2,0]/time_red[:,2,0], yerr=red_error,
#              marker=markers[1], color=colours[1], label=labels[2])
# plt.errorbar(N, time_red[:,3,0]/time_red[:,3,0], yerr=red_error,
#              marker=markers[1], color=colours[1], label=labels[3])
plt.plot(N, time_red[:,0,0]/time_red[:,0,0], marker=markers[0], color=colours[0], label=labels[0])
plt.plot(N, time_red[:,0,0]/time_red[:,1,0], marker=markers[1], color=colours[1], label=labels[1])
plt.plot(N, time_red[:,0,0]/time_red[:,2,0], marker=markers[2], color=colours[2], label=labels[2])
plt.plot(N, time_red[:,0,0]/time_red[:,3,0], marker=markers[3], color=colours[3], label=labels[3])
plt.plot(N, time_red[:,0,0]/time_red[:,4,0], marker=markers[4], color=colours[4], label=labels[4])
plt.plot(N, time_red[:,0,0]/time_red[:,5,0], marker=markers[5], color=colours[5], label=labels[5])
# plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
plt.xlabel('Datapoints')
plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
plt.xscale('log')
plt.legend()
plt.grid(True)
plt.savefig(maindir + 'red_speedup.eps', format='eps', dpi=1000)
plt.close()
