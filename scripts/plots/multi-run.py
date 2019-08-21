import numpy as np
import matplotlib.pyplot as plt
import math

maindir = '../../experiments/multi-run'
RUNS=1000
DIMS=3

colours = ('dodgerblue', 'mediumseagreen', 'orange', 'mediumslateblue')
markers = ('o', 'v', '1', '*')

# mean, std, error
stats=np.zeros((RUNS,DIMS))

for i in range(RUNS):
    file = "{0:s}/run-{1:04d}/postburn_samples.csv".format(maindir, i)

    data = np.genfromtxt(file, delimiter=',', skip_header=1)

    stats[i] = data.mean(0)

    if i%100==0:
        print('Run {0:d} completed.'.format(i))
# normalise
stats /= stats[:,0].mean()
# stats = (stats - stats.mean(0))/stats.std(0)

nc, binsc, patchesc = plt.hist(stats[:,0]*-10, bins=350, alpha=0.75, label=r'$\theta_0$')
ng, binsg, patchesg = plt.hist(stats[:,1]*-10, bins=350, alpha=0.75, label=r'$\theta_1$')
ns, binss, patchess = plt.hist(stats[:,2]*-10, bins=350, alpha=0.75, label=r'$\theta_2$')


nc = np.append(nc,0)
ng = np.append(ng,0)
ns = np.append(ns,0)


plt.figure()
plt.plot(binsc, nc, '-+', color=colours[0], label=r'$\theta_0$')
plt.xlabel('Value')
plt.ylabel('Frequency')
plt.legend()
plt.grid(True)
plt.savefig('histplot_theta0.eps', format='eps', dpi=1000)

plt.figure()
plt.plot(binsg, ng, '-+',color=colours[1], label=r'$\theta_1$')
plt.xlabel('Value')
plt.ylabel('Frequency')
plt.legend()
plt.grid(True)
plt.savefig('histplot_theta1.eps', format='eps', dpi=1000)

plt.figure()
plt.plot(binss, ns, '-+', color=colours[2], label=r'$\theta_2$')
plt.xlabel('Value')
plt.ylabel('Frequency')
plt.legend()
plt.grid(True)
plt.savefig('histplot_theta2.eps', format='eps', dpi=1000)
