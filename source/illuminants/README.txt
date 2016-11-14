Some notes on
A) Compilation
B) Execution of the code
C) Parameters
D) General remarks on our work in color constancy and image forensics
E) Referencing the code

--------------
A) COMPILATION
--------------
To compile the code, you require
- OpenCV (tested with 2.4.9)
- Boost (tested with 1.45)
- cmake (tested with 2.8.2)

We typically built the code on a debian squeeze Linux. The build process worked
also without modifications under Ubuntu Linux.

enter the root directory of the code (illum_maps). To build the code, execute these steps:
----------------snip------------------------
mkdir build
cd build
ccmake ../
----------------snap------------------------

The curses interface of cmake shows up. Press "c" to configure. If you compiled
OpenCV on your own, cmake probably complains that it did not find OpenCV. 
In this case, set the variable OpenCV_DIR to the directory in your OpenCV
installation that contains the file OpenCVConfig.cmake . This is typically
<opencv_install_dir>/share/OpenCV/

Set all switches that are “OFF” to “ON”, except "VOLE_SINGLE_TARGETS"

Press 'c' to configure the build again.

If you want to use a custom build of boost, press 't' (for detailed options) and fix the paths to the boost libraries, namely
Boost_PROGRAM_OPTIONS_LIBRARY
Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG
Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE
and also
Boost_INCLUDE_DIR
Boost_LIBRARY_DIRS
If your system provides a reasonably recent version of boost, this should not be necessary.

Press 'c' to configure the build again.
If everything went well, press 'g' to create Makefiles and to leave ccmake.

Type 'make' to build the code.
If everything goes well, you should have a binary 'vole' in the build directory.


------------
B) EXECUTION
------------
Invoking the binary vole with the parameter 'help', i.e.,
   ./vole --help
prints a list of available commands, namely 'felzenszwalb', 'lgrayworld' and
'liebv'.

In order to create an illuminant map, the image has first to be segmented into
superpixels. For that we use 'felzenszwalb', i.e., the graph-based segmentation
algorithm by Felzenszwalb and Huttenlocher. 
Then, an illuminant estimator has to be applied on each segment. We provide two
variants in this code: the generalized gray world algorithm ('lgrayworld') by
Joost van de Weijer et al., and illuminant estimation by voting ('liebv') which
I have proposed in earlier work.
An example image is provided in the subdirectory img/ (it is the same image as
in Fig. 2 of our paper "Exposing Digital Forgeries by Illumination Color
Classification").

In a nutshell, execute these steps:
  ./vole felzenszwalb  -I ../img/tangerines.jpg --deterministic_coloring -O segmented_tangerines.png --k 200 --max_intensity 255 
  ./vole lgrayworld --img.image ../img/tangerines.jpg -S segmented_tangerines.png -O lgrayworld_estimates.png --n 1 --p 1 --sigma 1
  ./vole liebv --img.image ../img/tangerines.jpg -S segmented_tangerines.png -O liebv_estimates.png --iebv_config ../lille/config/config_iebv_lenient.txt

The second and third command use the image and its segmentation to compute a
grayworld-based and an iebv-based illuminant map. Have a look at them, and also
have a look at the next section to better understand the output.

-------------
C) PARAMETERS
-------------
- Segmentation algorithm: The segmentation algorithm by Felzenszwalb and
  Huttenlocher is mainly guided by two parameters, namely 'k', which is for our
  task reasonable to choose between 150 and 500, and 'sigma', which may make
  sense to be chosen between 0.2 and 0.5. For details, please refer to
  Felzenszwalb's and Huttenlocher's original paper.
- Generalized gray world: the three determining parameters are "n", 'p', and
  'sigma'. 'n' may be 0, 1, or 2. 'p' may take any value, but generally
  speaking values over 7 or 8 do not change much anymore. 'sigma' can also take
  arbitrary values, but we only experimented with sigma=1. For details, please
  refer to Joost van de Weijer's original paper.
- Illuminant Estimation by Voting: the parameters are wrapped in a
  configuration file. In the sample lines above, this is
  ../lille/config/config_iebv_lenient.txt
  Two other configuration files are provided in the same directory, which are
  somewhat stricter w.r.t. which segments are admissible for estimating the
  illuminant color. We oftentimes used only either the 'lenient' configuration,
  or 'avg'. For details, please refer to our paper "Illuminant Color Estimation
  for Real-World Mixed-Illuminant Scenes".

---------------------------------------------------------------------
D) GENERAL REMARKS ON OUR WORK IN COLOR CONSTANCY AND IMAGE FORENSICS
---------------------------------------------------------------------
Estimating the color of the illuminant from a single image is an
underconstrained problem. Hence, each algorithm has to rely on certain
assumptions to make the problem tractable. Depending on how well the
assumptions are fulfilled in the actual image, the estimate is closer or less
close to the ground truth.
Operating on image regions instead of full images naturally makes the problem
harder, and hence the local estimation errors larger.

Several research groups investigate color constancy in a broader, computer
vision, context. There, the task is to be able to color-normalize an image as
if it were taken under a canonical white illuminant.
For image forensics, the goal is slightly different. We do not aim to get
accurate information that is close to the true illuminant. Instead, the aim is
to expose inconsistencies in the local color distribution of an image. Although
the absolute estimation errors per segment may still be considerable, the
forensic application shifts the focus to _relative_ errors between superpixels
in the image. In the line of this thought, we proposed color constancy
operators as statistical descriptors for image inhomogeneities in our 2010
Information Hiding paper "Scene Illumination as an Indicator of Image
Manipulation". In this paper, we consider inconsistencies of local illuminators
applied on segmentations of full images.

From a theoretical viewpoint, it is advantageous to consider only image regions
that consist of approximately the same underlying material. Hence, in our 2013
TIFS paper "Exposing Digital Image Forgeries by Illumination Color
Classification", we limited the investigation on comparing descriptors on face
regions of the image. Tiago Carvalho greatly extended this thought by devising
a machine-learning approach for automatically classifying spliced scenes of
faces.


-----------------------
E) REFERENCING THE CODE
-----------------------

In case that you use the code in your own scientific work, please cite one of
these papers:

  Christian Riess and Elli Angelopoulou: "Scene Illumination as an Indicator of
  Image Manipulation", Information Hiding Conference, June 2010, pp. 66-80.
or
  Tiago J. de Carvalho, Christian Riess, Elli Angelopoulou, Helio Pedrini,
  Anderson Rocha: "Exposing Digital Image Forgeries by Illumination Color
  Classification", IEEE Transactions on Information Forensics and Security,
  vol. 8, no. 7, pp. 1182-1194, July 2013.

