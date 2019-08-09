import numpy as np

def isclose(a, b, rel_tol=1e-09, abs_tol=0.0):
    return abs(a-b) <= max(rel_tol * max(abs(a), abs(b)), abs_tol)


fref =  '../out/ref/progress.csv'
ftest = '../out/test/progress.csv'

ref = np.genfromtxt(fref, delimiter=',', skip_header=1)
test = np.genfromtxt(ftest, delimiter=',', skip_header=1)

ctr = 0;
for i in range(0,ref.shape[0]):
    for j in range(0, ref.shape[1]):
        if not isclose(ref[i,j], test[i,j], rel_tol=1e-14, abs_tol=0.0):
            print("({0:d},{1:d}) differ: Ref={2:.17e}, Test={3:.17e}, Diff={4:.17e}".format(
                i, j, ref[i,j], test[i,j], abs(ref[i,j]-test[i,j])
            ))
            ctr += 1

if(ctr==0):
    print("PASS: Floating point numbers are withing the accepted tolerance")
