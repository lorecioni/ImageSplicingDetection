# Source code

This README file presents some notes about source code structure, compilation and execution.

##Project structure

###Folders

* **data**: the data folder, where trained models are stored
* **descriptors**: the set of used image color descriptors (*written in C/C++*)
* **illuminants**: the illuminant maps extraction algorithms (*written in C++*)
* **matlab**: the Matlab scripts (for evaluating performances)

###Scripts

* **classification.py**: a library wrapper for classification (kNN)
* **config.py**: the configuration file
* **descriptors.py**: the colors descriptors extraction script
* **distanceMetrics.py**: a set of functions for evaluating distances
* **experiments.py**: a set of functions used for experiments
* **faceSplicingDetector.py**: the face splicing detection module script
* **illuminantMaps.py**: the illuminant maps extraction script
* **loadDatasets.py**: utility functions for loading the training set
* **regionSplicingDetection.py**: the regional splicing detection module script
* **utils.py**: a set of utilities
* **main.py**: the entry script

##Installation

Before you can build this project, you must install and configure the following dependencies on your machine:

* **[OpenCV](http://opencv.org/)**: tested with >= 2.4
* **[Python 3](https://www.python.org/downloads/)**: linked with OpenCV (*cv2 module*)
* **[ImageMagick](https://www.imagemagick.org/script/index.php)**: tested with >= 6.0.3

###Getting started

1. Clone the git repository
```
git clone https://github.com/lorecioni/ImageSplicingDetection.git
```
