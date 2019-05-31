import numpy as np
from sklearn.decomposition import PCA
import preprocessing
import argparse
import util

args = util.parse_cmdline()
print(args)

if args.precision == 'double':
	type = 'f8'
else:
	type = 'f4'

print("Decompressing the dataset...", end='')
images = util.convert(args.data, args.labels, args.N)
print("\tDone")

print("Extracting data set with digits {0} or {1}...".format(args.classes[0], args.classes[1]))
images = preprocessing.filter_classes(images, args.classes, type)
print("Data set extracted.")

if args.shuffle != 0:
	print("Shuffling the data set...", end='')
	np.random.shuffle(images)
	print("\tDone")

Y = preprocessing.get_binary_labels(images, args.classes)
X = preprocessing.get_data(images, type)

N = X.shape[0]
N_train = int(N * args.train)

print("Splitting data set into train and test set...", end='')
X_train, Y_train, X_test, Y_test = preprocessing.split_dataset(X, Y, N_train)
print("\tDone.")

X_train, X_test = preprocessing.normazile_sets(X_train, X_test)

# perform pca
if args.pca != 0:
	print("Gennerating PCA components...", end='')
	pca = PCA(n_components=min(X_train.shape[0], args.pca))
	pca.fit(X_train)
	X_train = pca.transform(X_train)
	X_test  = pca.transform(X_test)
	print("\tDone.")

util.write_csv('../X_train{0}_{1}.csv'.format(args.classes[0], args.classes[1]), 'x', X_train, args.precision)
util.write_csv('../Y_train{0}_{1}.csv'.format(args.classes[0], args.classes[1]), 'y', Y_train, args.precision)
util.write_csv('../X_test{0}_{1}.csv'.format(args.classes[0], args.classes[1]), 'x', X_test, args.precision)
util.write_csv('../Y_test{0}_{1}.csv'.format(args.classes[0], args.classes[1]), 'y', Y_test, args.precision)




