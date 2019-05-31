import numpy as np
import argparse

def parse_cmdline():
	parser = argparse.ArgumentParser(description='Specify the parameters of the generated dataset')

	parser.add_argument('-data', dest='data', type=str, help='Specify the filename that contains the binary format data.', default='../generator/mnist1k-patterns-idx3-ubyte')
	parser.add_argument('-labels', dest='labels', type=str, help='Specify the filename that contains the binary format labels.', default='../generator/mnist1k-labels-idx1-ubyte')

	parser.add_argument('-classes', dest='classes', type=int, nargs='*', default=(4,9))

	parser.add_argument('-dim', dest='dim', type=int, help='an integer for dimensionality', default=784)
	parser.add_argument('-N', dest='N', type=int, help='an integer for dataset size', default=1000)

	parser.add_argument('-train', dest='train', type=float, help='Train data percentage', default=0.80)

	parser.add_argument('-precision', dest='precision', choices=['single', 'double'], help='Specify the precision to be used in the calculations', default='double')

	parser.add_argument('-pca', dest='pca', type=int, help='Select the number of pca dimensions. If set to 0, no pca is performed.', default=0)

	parser.add_argument('-shuffle', dest='shuffle', type=int, help='Choose if you want to shuffle the data set. Default is 0 i.e not.')
	args = parser.parse_args()

	return args

def convert(imgf, labelf, n):
	data = open(imgf, "rb")
	label = open(labelf, "rb")

	data.read(16)
	label.read(8)
	images = []

	for i in range(n):
		image = [ord(label.read(1))]
		for j in range(28*28):
			image.append(ord(data.read(1)))
		images.append(image)

	data.close()
	label.close()

	return np.array(images)


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
