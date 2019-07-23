import numpy as np
from sklearn.decomposition import PCA
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler
import preprocessing
import argparse
import util

args = util.parse_cmdline()
print(args)

if args.precision == 'double':
	type = 'f8'
else:
	type = 'f4'

# print("Decompressing the dataset...", end=" ")
print("Decompressing the dataset...")
images = util.convert(args.data, args.labels, args.N)
images = preprocessing.filter_classes(images, args.classes, type)

Y = preprocessing.get_binary_labels(images, args.classes)
X = preprocessing.get_data(images, type)
X_tr, X_ts, Y_tr, Y_ts = train_test_split(X, Y, test_size=(1-args.train), random_state=4)

mean = X_tr.mean()
std  = X_tr.std()
X_tr = (X_tr - mean) / std
X_ts = (X_ts - mean) / std

print(X_tr.mean(), X_tr.std())
print(X_ts.mean(), X_ts.std())

pca = PCA(args.pca).fit(X_tr)
# print(pca.explained_variance_ratio_)
# print(pca.explained_variance_ratio_.sum())
# print(pca.get_covariance())

X_tr_pca = pca.transform(X_tr)
X_ts_pca = pca.transform(X_ts)

X_tr_pca, X_ts_pca = preprocessing.append_bias(X_tr_pca, X_ts_pca)
print(X_tr_pca.shape)

util.write_csv('../X_train_{0}_{1}_{2}_{3}.csv'.format(args.classes[0], args.classes[1], X_tr_pca.shape[0], X_tr_pca.shape[1]-1), 'x', X_tr_pca, args.precision)
util.write_csv('../Y_train_{0}_{1}_{2}_{3}.csv'.format(args.classes[0], args.classes[1], X_tr_pca.shape[0], X_tr_pca.shape[1]-1), 'y', Y_tr, args.precision)
util.write_csv('../X_test_{0}_{1}_{2}_{3}.csv'.format(args.classes[0], args.classes[1], X_ts_pca.shape[0], X_ts_pca.shape[1]-1), 'x', X_ts_pca, args.precision)
util.write_csv('../Y_test_{0}_{1}_{2}_{3}.csv'.format(args.classes[0], args.classes[1], X_ts_pca.shape[0], X_ts_pca.shape[1]-1), 'y', Y_ts, args.precision)
