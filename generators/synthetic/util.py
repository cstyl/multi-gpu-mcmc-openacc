import argparse
import os
import numpy as np

def parse_cmdline():
	parser = argparse.ArgumentParser(description='Specify the parameters of the generated dataset')

	parser.add_argument('-dim', dest='dim', type=int, help='an integer for dimensionality', default=10)
	parser.add_argument('-Ntrain', dest='Ntrain', type=int, help='an integer for training dataset size', default=1000)
	parser.add_argument('-Ntest', dest='Ntest', type=int, help='an integer for test dataset size', default=200)

	parser.add_argument('-precision', dest='precision', choices=['single', 'double'], help='Specify the precision to be used in the calculations', default='double')

	parser.add_argument('-beta', dest='beta', type=float, nargs='*', default=(0.0))
	args = parser.parse_args()

	return args


def create_header(dim, caption):
	header = ""
	for i in range(dim):
		if i == dim - 1:
			header += "{0}{1}".format(caption, i)
		else:
			header += "{0}{1}, ".format(caption, i)

	return header

def write_csv(filename, caption, data, precision):
	header = create_header(data.shape[1], caption)
	# output values with the number of stable decimal digits for each precision
	if precision == 'float':
		np.savetxt(filename ,data, fmt='%.7f', delimiter=',', header=header)
	else:
		np.savetxt(filename ,data, fmt='%.16f', delimiter=',', header=header)

	print('File {0} generated.'.format(filename))
