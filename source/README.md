# Source code

This README file presents some notes about source code structure, compilation and execution.

## Project structure

### Folders

* **data**: the data folder, where trained models are stored
* **descriptors**: the set of used image color descriptors (*written in C/C++*)
* **illuminants**: the illuminant maps extraction algorithms (*written in C++*)
* **matlab**: the Matlab scripts (for evaluating performances)

### Scripts

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

## Installation

Before you can build this project, you must install and configure the following dependencies on your machine:

* **[OpenCV](http://opencv.org/)**: tested with >= 2.4
* **[Python 3](https://www.python.org/downloads/)**: linked with OpenCV (*cv2 module*)
* **[ImageMagick](https://www.imagemagick.org/script/index.php)**: tested with >= 6.0.3

### Getting started

1. Clone the git repository
```
git clone https://github.com/lorecioni/ImageSplicingDetection.git
```

1. Compile illuminant maps extraction code. Instructions can be found in the **illuminants** directory.

1. Compile each image color descriptors. For each folder contained in **descriptors**:

    1. Move into the descriptor **source** folder: ``` cd source```
    1.  ``` make clean```
    1.  ``` make```
    1.  ``` cd app```
    1.  ``` make [desc]_extraction``` where [desc] is the current descriptor name (e.g. ``` make acc_extraction```)

1. Setting up correct configurations
    1. Check that the the *config.txt* path (can be found in *illuminants/config.txt*) is correct in *config.py* property **config_iic**
    1. Set the *convert* binary of *ImageMagick* in *config.py* property *convertBinary*

## Running

Run the algorithm with **main.py** script passing arguments.

* **--face-detector** use the face detector module
* **--region-detector** use the region detector module
* **--detect** run forgery detection over an image
* **--train** train a selected detector
* **--evaluate** evaluate a trained detector over a validation set
* **--img** the path of the input image (required for detection)
* **--display-result** display resulting images
* **--verbose** print some additional running information

### Examples

#### Face forgery detection

Detect forgeries over human faces in the image using the face splicing detection module. Display the results in a visual way.

```
python3 main.py --face-detector --detect --img data/sample.png --display-result
```

#### Regional forgery detection

Detect forgeries in the image using the face splicing detection module. Display the results in a visual way.

```
python3 main.py --region-detector --detect --img data/sample.png --display-result
```
