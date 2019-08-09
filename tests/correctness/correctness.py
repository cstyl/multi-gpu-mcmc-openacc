import numpy as np
import isclose as prec

fref =  '../../out/ref/progress.csv'
ftest = '../../out/test/progress.csv'

ref = np.genfromtxt(fref, delimiter=',', skip_header=1)
test = np.genfromtxt(ftest, delimiter=',', skip_header=1)

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
    print("PASS: Floating point numbers are withing the accepted tolerance")
else:
    print("FAILED: Check experiment again")
