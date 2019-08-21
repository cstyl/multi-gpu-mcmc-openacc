from sklearn.linear_model import LogisticRegression
import numpy as np

datadir = '../../data/flights/minmax/'

trainx = np.genfromtxt(datadir + 'X_train.csv', delimiter=',', skip_header=1, max_rows=1000)
trainy = np.genfromtxt(datadir + 'Y_train.csv', delimiter=',', skip_header=1, max_rows=1000)
testx = np.genfromtxt(datadir + 'X_test.csv', delimiter=',', skip_header=1, max_rows=200)
testy = np.genfromtxt(datadir + 'Y_test.csv', delimiter=',', skip_header=1, max_rows=200)

clf = LogisticRegression(random_state=0, solver='lbfgs',
                         multi_class='ovr').fit(trainx, trainy)

pred = clf.predict(testx)
pred_prob = clf.predict_proba(testx)
score = clf.score(testx, testy)
print(score)
