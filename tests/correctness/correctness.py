import numpy as np
import isclose as prec
import argparse

def parse_cmdline():
    parser = argparse.ArgumentParser(description='Specify the parameters of the generated dataset')
    parser.add_argument('-r', dest='ref', type=str, help='Specify the reference file.', default='../../out/ref/progress.csv')
    parser.add_argument('-t', dest='test', type=str, help='Specify the filename to be tested against reference.', default='../../out/test/progress.csv')
    args = parser.parse_args()

    return args

header = ("Iter", "cPrior", "cLhood", "cPost", "pPrior", "pLhood", "pPost")

args = parse_cmdline()

fref =  args.ref
ftest = args.test

ref = np.genfromtxt(fref, delimiter=',', skip_header=1)
test = np.genfromtxt(ftest, delimiter=',', skip_header=1)

ctr = 0;
for i in range(0,ref.shape[0]):
    for j in range(0, ref.shape[1]):
        valid = prec.isclose(ref[i,j], test[i,j], rel_tol=1e-14, method='weak')
        if not valid:
            print("Iter: {0:d}: {1:s} differ:".format(i, header[j]), end ='')
            print("\tRef={0:.17e}".format(ref[i,j]), end ='')
            print("\tTest={0:.17e}".format(test[i,j]), end ='')
            print("\tDiff={0:.17e}".format(abs(ref[i,j]-test[i,j])))
            ctr += 1

if(ctr==0):
    print("PASS: Floating point numbers are withing the accepted tolerance")
else:
    print("FAILED: Check experiment again")
