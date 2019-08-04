import numpy as np

def generate_uniform_samples(low, high, N, dim):
    samples = np.random.uniform(low, high, (N, dim)).astype('f8')

    return samples

def get_model_parameters(range, dim):
    beta = generate_uniform_samples(range[0], range[1], 1, dim)

    return beta

def generate_data(range, N, dim):
    x = generate_uniform_samples(range[0], range[1], N, dim)

    return x

def add_bias(X_train, X_test):
    X_train = np.insert(X_train, 0, 1.0, axis=1)
    X_test = np.insert(X_test, 0, 1.0, axis=1)

    return (X_train, X_test)

def split_data(data, N_train):
	X_train = data[:N_train]
	X_test  = data[N_train:]

	return (X_train, X_test)

def normalize_sets(X_train, X_test):
	# evaluate mean and std for each dimension appart from bias dimension (i.e x0)
	mean = np.mean(X_train, axis=0)
	std  = np.std(X_train, axis=0)

	epsilon = 1e-17	# use to avoid division with zero
	# Normalize with zero mean and unit variance train set
	# and apply same transformation on test set
	X_train = (X_train - mean) / (std + epsilon)
	X_test  = (X_test  - mean) / (std + epsilon)

	return (X_train, X_test)

def get_labels(x, beta):
    y = np.dot(x,np.transpose(beta)).astype('f8')

    y[y < 0.0] = -1.0
    y[y >= 0.0] = 1.0

    return y
