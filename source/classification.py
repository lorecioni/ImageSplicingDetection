'''
Classification utilities and classes

@author: Lorenzo Cioni
'''

from sklearn import neighbors
from sklearn.model_selection import KFold

class TrainingSample:
    def __init__(self, data, label):
        self.label = label
        self.feature = data

    def __str__(self):
        return self.label + ":" + self.feature + "\n"

    @staticmethod
    def fromTxt(txt):
        data = txt.split(":")
        return TrainingSample(data[1], data[0])


'''
K fold splitting
'''
def splitDataset(data, k):
    kf = KFold(n_splits = k, shuffle=True)
    return kf.split(data)


'''Classifier abstract class'''
class Classifier:
    def train(self):
        raise NotImplementedError("Train method not implemented yet")
    def predict(self):
        raise NotImplementedError("Predict method not implemented yet")


'''KNN classifier class'''
class KNNClassifier(Classifier):
    def __init__(self, k, weights = 'uniform'):
        self.K = k
        self.weights = weights

    def train(self, data, labels):
        self.clf = neighbors.KNeighborsClassifier(self.K, self.weights)
        self.clf.fit(data, labels)

    def predict(self, data):
        return self.clf.predict(data)
