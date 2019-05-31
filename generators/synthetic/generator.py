# A linear model y = sign(b0 + b1*x1 + ... + bn*xn) is used where:
#   - a set of N independent data x is generated randomly
#   - the data set y is simulated using the parameters b
import numpy as np
import preprocessing
import util
import synthetic as s



args = util.parse_cmdline()
print(args)
beta_range = [-20.0, 20.0]
x_range = [-100.0, 100.0]

N_train = int(args.N * args.train)

# generate model parameters
beta = s.get_model_parameters(beta_range, args.dim+1)

# generate data and labels
X, Y = s.generate_dataset(beta, x_range, args.N, args.dim)

X_train, Y_train, X_test, Y_test = preprocessing.split_dataset(X, Y, N_train)

X_train, X_test = preprocessing.normazile_sets(X_train, X_test)

util.write_csv('./beta_{0}.csv'.format(args.dim), 'beta', beta, args.precision)
util.write_csv('./X_train_{0}_{1}.csv'.format(args.N, args.dim), 'x', X_train, args.precision)
util.write_csv('./Y_train_{0}_{1}.csv'.format(args.N, args.dim), 'y', Y_train, args.precision)
util.write_csv('./X_test_{0}_{1}.csv'.format(args.N, args.dim), 'x', X_test, args.precision)
util.write_csv('./Y_test_{0}_{1}.csv'.format(args.N, args.dim), 'y', Y_test, args.precision)
