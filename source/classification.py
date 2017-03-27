'''
Classification utilities and classes

@author: Lorenzo Cioni
'''

from sklearn import neighbors
from sklearn.model_selection import KFold
from sklearn.externals import joblib

class FaceTrainingSample:
    def __init__(self, data, label, first = None, second = None, filename = None):
        self.label = label
        self.feature = data
        self.first = first
        self.second = second
        self.filename = filename

    def __str__(self):
        return str(self.label) + ":" + self.feature + "\n"

    @staticmethod
    def fromTxt(txt):
        data = txt.split(":")
        return FaceTrainingSample(data[1], int(data[0]))


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
    def store(self, path):
        raise NotImplementedError("Store method not implemented yet")


'''KNN classifier class'''
class KNNClassifier(Classifier):
    def __init__(self, k = 5, weights = 'distance'):
        self.K = k
        self.weights = weights
        self.clf = None

    def train(self, data, labels):
        self.clf = neighbors.KNeighborsClassifier(self.K, self.weights)
        self.clf.fit(data, labels)

    def predict(self, data, probability = False):
        if not probability:
            prediction = self.clf.predict(data)
        else:
            prediction = self.clf.predict_proba(data)
        if len(prediction) > 1:
            return prediction
        else:
            return int(prediction[0])


    def store(self, path):
        joblib.dump(self.clf, path)

    @staticmethod
    def load(path):
        classifier = KNNClassifier()
        classifier.clf = joblib.load(path)
        return classifier


