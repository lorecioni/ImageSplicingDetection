'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import argparse
import faceSplicingDetector
import regionSplicingDetection
import sys
import loadDatasets

__version__ = '0.5.1'
__date__ = '2016-10-14'
__updated__ = '2017-02-09'

def main():
    print('ImageSplicingDetection v.' + str(__version__))
    print('Creation date: ' + __date__ + ', last update: ' + __updated__)
    parser = argparse.ArgumentParser()

    parser.add_argument("--face-detector", help="use face detector", dest='face_detector', action='store_true')
    parser.add_argument("--region-detector", help="use region detector", dest='region_detector', action='store_true')

    parser.add_argument("--train", help="train the model for further splicing detection", dest='train', action='store_true')
    parser.add_argument("--detect", help="detect splice over an image", dest='detect', action='store_true')
    parser.add_argument("--evaluate", help="evaluate trained models over a set of images", dest='evaluate', action='store_true')

    parser.add_argument("--output-mask", help="output mask path", dest="output_mask")

    parser.add_argument("--display-result", help="display mask", dest="display_result", action="store_true")

    parser.add_argument("--crossvalidate", help="cross-validate the dataset", dest='cross_validation', action='store_true')
    parser.add_argument("--extract-single-features", help="extract feature vector for a specific image", dest='extract_single_features', action='store_true')
    parser.add_argument("--no-extract-features", help="no extract all training images features", dest='extract_features', action='store_false')
    parser.add_argument("--no-extract-maps", help="no extract all training images features", dest='extract_maps', action='store_false')
    parser.add_argument("--img", help="the path of the suspicious image")

    parser.add_argument("--euclidean-distances", help="visualize distances", dest='evaluate_eucl_distances', action='store_true')
    parser.add_argument("--heat-map", help="display the heat map between GGE and IIC maps", dest='heat_map', action='store_true')
    parser.add_argument("--verbose", help="display all messages", dest='verbose', action='store_true')

    #Setting defaults
    parser.set_defaults(heat_map = False)
    parser.set_defaults(verbose = False)
    parser.set_defaults(train = False)
    parser.set_defaults(detect = False)
    parser.set_defaults(evaluate=False)
    parser.set_defaults(extract_maps = True)
    parser.set_defaults(extract_features = True)
    parser.set_defaults(cross_validation = False)
    parser.set_defaults(extract_single_features = False)
    parser.set_defaults(evaluate_eucl_distances = False)
    parser.set_defaults(display_result=False)
    parser.set_defaults(output_mask = "output_mask.png")
    parser.set_defaults(face_detector = False)
    parser.set_defaults(region_detector = False)

    args = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)

    if not args.face_detector and not args.region_detector:
        print("No detector selected: choose --face-detector or --region-detector option.")
        parser.print_usage()
        sys.exit(1)

    #Initialize splicing detector class
    if args.face_detector:
        detector = faceSplicingDetector.FaceSplicingDetector(args.extract_maps, args.extract_features, args.cross_validation, args.verbose, args.display_result)

    elif args.region_detector:
        detector = regionSplicingDetection.RegionSplicingDetector(args.extract_maps, args.extract_features, args.cross_validation, args.verbose, args.display_result)
        #detector = splicingDetection.SplicingDetection(args.extract_maps, args.extract_features, args.cross_validation, args.verbose, args.heat_map)

    if args.train:
        #Training the model
        images, labels = loadDatasets.load()
        if len(images) > 0:
            detector.train(images, labels)

    elif args.detect:
        #Detecting splice over a selected image
        if len(args.img) > 0:
            detector.detect(args.img, args.output_mask)
        else:
            print('No image selected for splicing detection. Must specify the --img argument.')

    elif args.extract_single_features:
        #Extract feature vector for a selected image
        if len(args.img) > 0:
            print('Extracting image feature vector froma single image')
            detector.extractFeatures(args.img)
        else:
            print('No image selected for splicing detection. Must specify the --img argument.')

    elif args.evaluate_eucl_distances:
        # Evaluate euclidean distances between each image IMs
        images, _ = loadDatasets.load()
        detector.evaluateEuclideanDistances(images)

    elif args.evaluate:
        images, labels = loadDatasets.load()
        if len(images) > 0:
            detector.evaluate(images, labels)


if __name__ == '__main__':
    main()
