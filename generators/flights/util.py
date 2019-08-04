import argparse
import os
import numpy as np

def parse_cmdline():
	parser = argparse.ArgumentParser(description='Specify the parameters of the generated dataset')

	parser.add_argument('-i', dest='filename', type=str, help='Specify the filename that flights dataset.', default='./Airline/1995.csv')

	parser.add_argument('-o', dest='outdir', type=str, help='Specify the output directory to place the generated datasets.', default='../../data/flights/minmax')

	parser.add_argument('-r', dest='ratio', type=float, help='Define a train-test split ratio', default=0.8)
	parser.add_argument('-s', dest='scaler', choices=['MinMax', 'MaxAbs', 'Standard', 'Robust', 'Normal', 'QuantileGaussian', 'QuantileUniform', 'Power'], help='Choose the scaler to be applied on the data', default='MinMax')

	parser.add_argument('-p', dest='precision', choices=['single', 'double'], help='Specify the precision to be used in the calculations', default='double')

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
