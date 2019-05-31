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

def get_binary_labels(dataset, classes):
	labels = np.zeros((dataset.shape[0], 1))
	labels[:,0] = dataset[:,0]
	
	# convert labels to -1 and 1
	labels[labels==classes[0]] = -1
	labels[labels==classes[1]] = +1

	return labels

def get_data(dataset, type):
	# remove labels from data and add the bias term
	data = np.delete(dataset, 0, 1)
	data = np.insert(data, 0, 1, axis=1) # add 1 for the bias in each point
	data = data.astype(type)

	return data

def normazile_sets(X_train, X_test):
	# evaluate mean and std for each dimension appart from bias dimension (i.e x0)
	mean = np.mean(X_train[:,1:], axis=0)
	std  = np.std(X_train[:,1:], axis=0)

	epsilon = 1e-17	# use to avoid division with zero
	# Normalize with zero mean and unit variance train set 
	# and apply same transformation on test set
	X_train[:,1:] = (X_train[:,1:] - mean) / (std + epsilon)
	X_test[:,1:]  = (X_test[:,1:]  - mean) / (std + epsilon)

	return (X_train, X_test)