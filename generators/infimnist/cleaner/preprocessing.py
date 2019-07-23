import numpy as np

def split_dataset(data, labels, N_train):
	X_train = data[:N_train]
	Y_train = labels[:N_train]
	X_test  = data[N_train:]
	Y_test  = labels[N_train:]
	# print(X_train.shape, Y_train.shape, X_test.shape, Y_test.shape)

	return (X_train, Y_train, X_test, Y_test)

def filter_classes(images, classes, type):
	labels = images[:,0]

	for i, label in enumerate(classes):
		features = images[labels==label]
		print('\tNumber of {0}s:  {1}'.format(label, features.shape[0]))

		if i==0:
			subset = features
		else:
			subset = np.concatenate((subset,features), axis=0)

	return subset

def get_labels(dataset):
	labels = np.zeros((dataset.shape[0], 1))
	labels[:,0] = dataset[:,0]

	return labels

def get_binary_labels(dataset, classes):
	labels = np.zeros((dataset.shape[0], 1))
	labels[:,0] = dataset[:,0]

	# convert labels to -1 and 1
	labels[labels==classes[0]] = -1
	labels[labels==classes[1]] = +1

	return labels

def get_data(dataset, type):
	# remove labels from data
	data = np.delete(dataset, 0, 1).astype(type)

	return data

def normazile_sets(X_train, X_test):
	# evaluate mean and std for all images appart from bias dimension (i.e x0)
	mean = np.mean(X_train)
	std  = np.std(X_train)
	epsilon = 1e-17	# use to avoid division with zero
	# Normalize by subtracting mean and dividing by variance
	X_train = (X_train - mean) / (std + epsilon)
	X_test  = (X_test  - mean) / (std + epsilon)

	return (X_train, X_test)

def get_sum_of_variance(X):
	var = np.var(X, axis=0)

	return(var)

def append_bias(X_train, X_test):

	X_train = np.insert(X_train, 0, 1, axis=1) # add 1 for the bias in each point
	X_test = np.insert(X_test, 0, 1, axis=1) # add 1 for the bias in each point

	return (X_train, X_test)
