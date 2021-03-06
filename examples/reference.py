import arboretum
import numpy as np
from sklearn.datasets import load_boston
import xgboost


def rmse(y, y_hat):
    diff = np.power(y - y_hat, 2)
    return np.sqrt(np.sum(diff))

# load test data
boston = load_boston()
n = 10000

# create data matrix
data = arboretum.DMatrix(boston.data[0:n], y=boston.target[0:n])
y = boston.target[0:n]

# init model
model = arboretum.Garden('reg:linear', data, 6, 2, 1, 0.5)

# grow trees
for i in xrange(5):
    model.grow_tree()

# predict on train data set
pred = model.predict(data)

# print first n records
print pred[0:10]

#RMSE
print rmse(pred, y)


# xgboost as refernce value
data1 = arboretum.DMatrix(boston.data[0:n], y=boston.target[0:n])
y1 = boston.target[0:n]

pred = model.predict(data1)

print pred[0:10]
print rmse(pred, y1)

mat = xgboost.DMatrix(boston.data[0:n], label=boston.target[0:n])
param = {'max_depth':5,
         'silent':True, 'objective':'reg:linear' }
param['nthread'] = 1
param['min_child_weight'] = 2
param['colspan_by_tree'] = 1.0
param['colspan_by_level'] = 1.0
param['eval_metric'] = 'rmse'
param['lambda'] = 0.0
param['eta'] = 1.0
param['gamma'] = 0.0
param['alpha'] = 0.0

model = xgboost.train(param, mat, 5)
pred = model.predict(mat)
print pred[0:10]
print boston.target[0:10]

print rmse(pred, y)
