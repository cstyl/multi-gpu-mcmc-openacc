import numpy as np
import matplotlib.pyplot as plt
import math

def speed_up_error(T1, Tp, dT1, dTp):

    return np.sqrt(np.power(dT1/Tp,2) + np.power(T1*dTp/np.power(Tp,2),2))

maindir = '../../experiments/'
omp_timing = maindir + 'omp/timing.csv'
mpi_timing = maindir + 'mpi/timing.csv'

N_LABEL=('10k', '100k', '1M', '10M')
N=(10000, 100000, 1000000, 10000000)
DIMS=10
CORES=(1, 2, 4, 8, 10, 15, 17, 21, 24, 28, 32, 36)


labels = ('mcmc', 'lhood', 'mvmul', 'reduction')
colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

data = np.genfromtxt(omp_timing, delimiter=',', skip_header=1)
data_mpi = np.genfromtxt(mpi_timing, delimiter=',', skip_header=1)

# 2 is for mean, std
time_mcmc = np.zeros((len(N), len(CORES), 2))
time_lhood = np.zeros((len(N), len(CORES), 2))
time_mvmul = np.zeros((len(N), len(CORES), 2))
time_red = np.zeros((len(N), len(CORES), 2))

mpi_mcmc = np.zeros((len(N), 2))
mpi_lhood = np.zeros((len(N), 2))
mpi_mvmul = np.zeros((len(N), 2))
mpi_red = np.zeros((len(N), 2))


for nidx,n in enumerate(N):

    block_N_mpi = data_mpi[:,3]
    data_N_mpi = data_mpi[block_N_mpi==n,:]

    block_core_mpi = data_N_mpi[:,1]
    data_core_mpi = data_N_mpi[block_core_mpi==1,:]

    mpi_mcmc[nidx,0] = data_core_mpi[:,7].mean()- data_core_mpi[:,14].mean()
    mpi_mcmc[nidx,1] = data_core_mpi[:,7].std()- data_core_mpi[:,14].std()

    mpi_lhood[nidx,0] = data_core_mpi[:,8].mean()
    mpi_lhood[nidx,1] = data_core_mpi[:,8].std()

    mpi_mvmul[nidx,0] = data_core_mpi[:,9].mean()
    mpi_mvmul[nidx,1] = data_core_mpi[:,9].std()

    mpi_red[nidx,0] = data_core_mpi[:,10].mean()
    mpi_red[nidx,1] = data_core_mpi[:,10].std()

    block_N = data[:,3]
    data_N = data[block_N==n,:]

    for cidx,c in enumerate(CORES):

        block_core = data_N[:,2]
        data_core = data_N[block_core==c,:]


        time_mcmc[nidx,cidx,0] = data_core[:,7].mean()- data_core[:,14].mean()
        time_mcmc[nidx,cidx,1] = data_core[:,7].std()- data_core[:,14].std()

        time_lhood[nidx,cidx,0] = data_core[:,8].mean()
        time_lhood[nidx,cidx,1] = data_core[:,8].std()

        time_mvmul[nidx,cidx,0] = data_core[:,9].mean()
        time_mvmul[nidx,cidx,1] = data_core[:,9].std()

        time_red[nidx,cidx,0] = data_core[:,10].mean()
        time_red[nidx,cidx,1] = data_core[:,10].std()

    time_mcmc[nidx,0,0] = mpi_mcmc[nidx,0]
    time_mcmc[nidx,0,1] = mpi_mcmc[nidx,1]

    time_lhood[nidx,0,0] = mpi_lhood[nidx,0]
    time_lhood[nidx,0,1] = mpi_lhood[nidx,1]

    time_mvmul[nidx,0,0] = mpi_mvmul[nidx,0]
    time_mvmul[nidx,0,1] = mpi_mvmul[nidx,1]

    time_red[nidx,0,0] = mpi_red[nidx,0]
    time_red[nidx,0,1] = mpi_red[nidx,1]

    mcmc_error = speed_up_error(time_mcmc[nidx,0,0], time_mcmc[nidx,:,0],
                                time_mcmc[nidx,0,1], time_mcmc[nidx,:,1])

    lhood_error = speed_up_error(time_lhood[nidx,0,0], time_lhood[nidx,:,0],
                                time_lhood[nidx,0,1], time_lhood[nidx,:,1])

    mvmul_error = speed_up_error(time_mvmul[nidx,0,0], time_mvmul[nidx,:,0],
                                time_mvmul[nidx,0,1], time_mvmul[nidx,:,1])

    red_error = speed_up_error(time_red[nidx,0,0], time_red[nidx,:,0],
                                time_red[nidx,0,1], time_red[nidx,:,1])

    plt.figure()
    plt.errorbar(CORES, time_mcmc[nidx,:,0], yerr=time_mcmc[nidx,:,1],
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(CORES, time_lhood[nidx,:,0], yerr=time_lhood[nidx,:,1],
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(CORES, time_mvmul[nidx,:,0], yerr=time_mvmul[nidx,:,1],
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(CORES, time_red[nidx,:,0], yerr=time_red[nidx,:,1],
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.xlabel('Cores')
    plt.ylabel('Execution Time (s)')
    # plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(maindir + 'omp/omp_runtime_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()


    plt.figure()
    plt.errorbar(CORES, mpi_mcmc[nidx,0]/time_mcmc[nidx,:,0], yerr=mcmc_error,
                 marker=markers[0], color=colours[0], label=labels[0])

    plt.errorbar(CORES, mpi_lhood[nidx,0]/time_lhood[nidx,:,0], yerr=lhood_error,
                 marker=markers[1], color=colours[1], label=labels[1])

    plt.errorbar(CORES, mpi_mvmul[nidx,0]/time_mvmul[nidx,:,0], yerr=mvmul_error,
                 marker=markers[2], color=colours[2], label=labels[2])

    plt.errorbar(CORES, mpi_red[nidx,0]/time_red[nidx,:,0], yerr=red_error,
                 marker=markers[3], color=colours[3], label=labels[3])
    plt.plot(CORES, CORES, linestyle='--', color='black', label='ideal')
    plt.xlabel('Cores')
    plt.ylabel('Speed Up ' + r'$\frac{T_{1}}{T_{c}}$' + ' (Times)')
    # plt.yscale('log')
    plt.legend()
    plt.grid(True)
    plt.savefig(maindir + 'omp/omp_speedup_' + N_LABEL[nidx] + '.eps', format='eps', dpi=1000)
    plt.close()
