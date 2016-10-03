'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import argparse
import splicingDetection

__version__ = 0.1
__date__ = '2016-10-03'
__updated__ = '2016-10-03'

def main():
    print('ImageSplicingDetection v.' + str(__version__) + '\n')
    parser = argparse.ArgumentParser()
    parser.add_argument("--img", help="the path of the suspicious image")
    args = parser.parse_args()
    splicingDetection.detectSplice(args.img)

if __name__ == '__main__':
    main()
