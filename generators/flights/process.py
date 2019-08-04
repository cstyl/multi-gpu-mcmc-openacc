import pandas as pd
import numpy as np
import util
import scale
import covariates as cov
from sklearn.model_selection import train_test_split
import math
import os

args = util.parse_cmdline()
print(args)

if not os.path.exists(args.outdir):
    os.makedirs(args.outdir)
    print("Created {0}".format(args.outdir))

# Read data from csv and filter only the required columns and remove empty or NaN fields
filter_names = ['Distance', 'ArrDelay', 'CRSDepTime', 'DayOfWeek', 'DepTime']
data = pd.read_csv(args.filename, delimiter=',', usecols=filter_names).dropna(axis=0)

covariate = pd.DataFrame({'Late':     data.ArrDelay.apply(cov.calculate_late),
                          'Night':    data.CRSDepTime.apply(cov.calculate_night),
                          'Weekend':  data.DayOfWeek.apply(cov.calculate_weekend),
                          'DepHour':  data.DepTime.apply(cov.calculate_dephour),
                          'Distance': data.Distance.apply(cov.calculate_distance)
                          }).values

Y = covariate[:,0]
X = covariate[:,1:covariate.shape[1]]
X_tr, X_ts, Y_tr, Y_ts = train_test_split(X, Y, test_size=(1-args.ratio), random_state=4)
# Scale datasets
scaling = scale.scale(args.scaler, X_tr)
Xtr_scale = scaling.transform(X_tr)
Xts_scale = scaling.transform(X_ts)
# Add bias
Xtr_scale = np.insert(Xtr_scale, 0, 1.0, axis=1)
Xts_scale = np.insert(Xts_scale, 0, 1.0, axis=1)

Y_tr = Y_tr.reshape((Y_tr.shape[0], 1))
Y_ts =Y_ts.reshape((Y_ts.shape[0], 1))
util.write_csv(args.outdir + '/X_train.csv', 'x', Xtr_scale, args.precision)
util.write_csv(args.outdir + '/Y_train.csv', 'y', Y_tr, args.precision)
util.write_csv(args.outdir + '/X_test.csv', 'x', Xts_scale, args.precision)
util.write_csv(args.outdir + '/Y_test.csv', 'y', Y_ts, args.precision)
