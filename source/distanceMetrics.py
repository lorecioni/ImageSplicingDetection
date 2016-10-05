'''
Created on 05 ott 2016

@author: lorenzocioni

Different matrics for evaluating distances bewteen eigenvalues
'''
import math

def linearDistance(a, b):
    return a - b

def quadraticDistance(a, b):
    return pow((a - b), 2)

def logarithmicDistance(a, b):
    return math.log(abs(a - b))

def squareRootDistance(a, b):
    return math.sqrt(abs(a - b))

def cubicRootDistance(a, b):
    return pow((a - b), 1/3)
    
