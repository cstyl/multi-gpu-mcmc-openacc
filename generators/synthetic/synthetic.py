import numpy as np

def generate_dataset(beta, range, N, dim):
    x = get_data(range, N, dim)
    y = get_labels(x, beta)

    return(x, y)

def generate_uniform_samples(low, high, N, dim):
    samples = np.random.uniform(low, high, (N, dim)).astype('f8')

    return samples

def get_model_parameters(range, dim):
    beta = generate_uniform_samples(range[0], range[1], 1, dim)

    return beta

def get_data(range, N, dim):
    x = generate_uniform_samples(range[0], range[1], N, dim)
    x = np.insert(x, 0, 1.0, axis=1)  # add 1 for the bias in each point

    return x

def get_labels(x, beta):
    y = np.dot(x,np.transpose(beta)).astype('f8')

    y[y < 0.0] = -1.0
    y[y >= 0.0] = 1.0

    return y
