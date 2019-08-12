import numpy as np
import isclose as prec

fref =  '../../out/ref/progress.csv'
arch_dir = '../../out/archive'
versions = ('gcc-serial', 'pgi-serial', 'acc-simple-host',
            'acc-simple-gpu-kernel', 'acc-simple-gpu-parloop',
            'acc-data-host', 'acc-data-gpu',
            'test-gpu-1', 'test-gpu-2', 'test-gpu-3', 'test-gpu-4')

ref = np.genfromtxt(fref, delimiter=',', skip_header=1)

for ftest in versions:
    filename = "{0}/{1}/progress.csv".format(arch_dir, ftest)
    test = np.genfromtxt(filename, delimiter=',', skip_header=1)
    ctr = 0;
    for i in range(0,ref.shape[0]):
        for j in range(0, ref.shape[1]):
            valid = prec.isclose(ref[i,j], test[i,j], rel_tol=1e-14, method='weak')
            if not valid:
                print("({0:d},{1:d}) differ:".format(i, j), end ='')
                print("\tRef={0:.17e}".format(ref[i,j]), end ='')
                print("\tTest={0:.17e}".format(test[i,j]), end ='')
                print("\tDiff={0:.17e}".format(abs(ref[i,j]-test[i,j])))
                ctr += 1

    if(ctr==0):
        print("PASS:\t{0:s}".format(ftest))
    else:
        print("FAILED:\t{0:s}".format(ftest))
