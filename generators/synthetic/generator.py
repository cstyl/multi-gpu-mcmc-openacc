# A linear model y = sign(b0 + b1*x1 + ... + bn*xn) is used where:
#   - a set of N independent data x is generated randomly
#   - the data set y is simulated using the parameters b
import numpy as np
import util
import synthetic as s

args = util.parse_cmdline()
print(args)
beta_range = [-20.0, 20.0]
x_range = [-100.0, 100.0]

N_train = int(args.Ntrain)
N_test = int(args.Ntest)
N = N_train + N_test

# choose model parameters from the command line
beta = np.array(args.beta)
beta = np.reshape(beta, (1, beta.shape[0]))
if beta[0,0] == 0 and beta.shape == (1,1):
    # generate model parameters
    beta = s.get_model_parameters(beta_range, args.dim+1)

# generate data
X = s.generate_data(x_range, N, args.dim)
# split data into train and test and normalize
X_train, X_test = s.split_data(X, N_train)
X_train, X_test = s.normazile_sets(X_train, X_test)
# add bias
X_train, X_test = s.add_bias(X_train, X_test)

Y_train = s.get_labels(X_train, beta)
Y_test = s.get_labels(X_test, beta)

util.write_csv('./beta_{0}.csv'.format(args.dim), 'beta', beta, args.precision)
util.write_csv('./X_train_{0}_{1}.csv'.format(N, args.dim), 'x', X_train, args.precision)
util.write_csv('./Y_train_{0}_{1}.csv'.format(N, args.dim), 'y', Y_train, args.precision)
util.write_csv('./X_test_{0}_{1}.csv'.format(N, args.dim), 'x', X_test, args.precision)
util.write_csv('./Y_test_{0}_{1}.csv'.format(N, args.dim), 'y', Y_test, args.precision)
