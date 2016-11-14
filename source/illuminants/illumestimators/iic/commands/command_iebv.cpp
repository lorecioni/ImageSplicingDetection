#include <iostream>
#include <sstream>
#include <opencv2/highgui/highgui.hpp>
#ifdef WITH_QT
#include <QTime>
#endif

#include "command_iebv2.h"
#include "iic.h"
#include "iebv.h"
#include "tan.h"
#include "felzenszwalb/felzenszwalbsegmentation.h"
#include "patches/patchessegmentation.h"
#include "veksler/vekslersegmentation.h"
//#include "normal_image.h"
#include "illuminantestimator.h"
#include "fusion/histogramVoting/histogramVotingFusion.h"
#include "plotter.h"

namespace iic {


CommandIebv::CommandIebv()
 : vole::Command(
		"iebv2",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de")
{
}

int CommandIebv::execute( illumestimators::Illum& estimate )
{
	int ret = execute();
	estimate = estimate_;
	return ret;
}

int CommandIebv::execute() {
	if (config.verbosity > 0) std::cout << "Illuminant Estimation by Voting" << std::endl;

	// guarantee that all verbosities are equivalent.
	// FIXME: Necessary, as IebvConfig, FelzenszwalbConfig, and PatchesConfig are all derived
	//        from Config, creating multiple different instances of a verbosity value, that
	//        should be equal. No idea how to fix this minor design flaw by redesigning, properly.
	if (config.verbosity != config.seg_felz.verbosity) {
		config.seg_felz.verbosity = config.verbosity;
	}
	if (config.verbosity != config.seg_patches.verbosity) {
		config.seg_patches.verbosity = config.verbosity;
	}

	if ((config.input_file.length() < 1) && (config.input_file_16_bit.length() < 1)) {
		std::cerr << "no input file given (-I or -J), aborted." << std::endl;
		return 1;
	}
	cv::Mat_<cv::Vec3d> normalized_image;
	if (config.input_file.length() > 0) {
		normalized_image = illumestimators::IlluminantEstimator::cleanlyReadImage(config.input_file, "rgb", 0);
	} else {
		normalized_image = illumestimators::IlluminantEstimator::cleanlyReadImage(config.input_file_16_bit, "rgb", 0);
	}

	illumestimators::IlluminantEstimator *estimator;
	if (config.with_hough) {
		Tan *t = new Tan(config);
		t->createIICSpace(normalized_image);
		estimator = t;
	} else {
		Iebv *i = new Iebv(config);
		i->createIICSpace(normalized_image);
		estimator = i;
	}
	/*
	Iebv iebv(config);
	iebv.createIICSpace(normalized_image);
	Tan tan(config);
	tan.createIICSpace(normalized_image);
	*/

	std::string outfile;
	// TODO: write somewhere else then /tmp, i.e. -O option
	{ std::stringstream s; s << "/tmp/iic_iebv_import_" << config.input_intensity_max << ".png"; outfile = s.str(); }
	cv::imwrite(outfile, normalized_image*255);

	int error_stat[5];
	error_stat[0] = error_stat[1] = error_stat[2] = error_stat[3] = error_stat[4] = 0;

#define NEW_IEBV 0
#if NEW_IEBV
	cv::Mat_<cv::Vec3b> color_segmented(normalized_image.rows, normalized_image.cols, cv::Vec3b(0, 0, 0));
	superpixels::VekslerSegmentation segV(config.seg_veks);
	std::vector<superpixels::Superpixel> segmentation = segV.superpixels((cv::Mat_<cv::Vec3d>)normalized_image);
	srand(time(NULL)+rand());

	cv::Mat_<unsigned char> confidence(normalized_image.rows, normalized_image.cols, (unsigned char)0);
	int overall_max = 0;

	for (int i = 0; i < (int) segmentation.size(); i++) {
		std::vector<cv::Point> &coordinates = segmentation[i].coordinates;

		// confidence
		double max = 0;
		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			for (int c = 0; c < 3; ++c)
				if (normalized_image[point.y][point.x][c] > max)
					max = normalized_image[point.y][point.x][c];
		}
		if (max > overall_max) overall_max = max;
		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			confidence[point.y][point.x] = max;
		}
		
//		cv::Vec3d col = iebv.estimateIlluminant(normalized_image, coordinates);
//		illumestimators::Illum col = iebv.estimateIlluminant(normalized_image, coordinates);
		illumestimators::Illum col = estimator->estimateIlluminant(normalized_image, coordinates);
		// TODO: next line will be a problem: error can be anything between 0 and 7! Just taking
		// log2(error+0.5) will not give the full information. E.g. error = 7 should result in
		// error_stat[0]++, error_stat[1]++, error_stat[2]++
//		if (iebv.error() > 0) { error_stat[(int)(log2(iebv.error()+0.5))]++; }
//		if ((col[0] < 0) || (col[1] < 0) || (col[2] < 0) || (col[0] > 1) || (col[1] > 1) || (col[2] > 1)) { col[0] = col[1] = col[2] = 0; }
//		if ((col[0] < 0) || (col[1] < 0) || (col[2] < 0)) { col[0] = 1; error_stat[3]++; }
//		if ((col[0] > 1) || (col[1] > 1) || (col[2] > 1)) { col[2] = 1; error_stat[4]++; }

		for (int j = 0; j < (int)coordinates.size(); ++j) {
			cv::Point &point = coordinates[j];
			double sum = 0;
			for (int c = 0; c < 3; ++c) sum += col[c];
			for (int c = 0; c < 3; ++c)
				color_segmented[point.y][point.x][c] = col[c]*255/sum;
		}
	}

	// confidence
	if (overall_max <= std::numeric_limits<double>::epsilon()) overall_max = 1;
	for (int y = 0; y < confidence.rows; ++y) {
		for (int x = 0; x < confidence.cols; ++x) {
			confidence[y][x] = confidence[y][x]*255.0/overall_max;
		}
	}

	// write image w/ segmentation indices
	cv::Mat_<cv::Vec3b> segmentation_index(normalized_image.rows, normalized_image.cols, cv::Vec3b(255, 255, 255));
	for (int i = 0; i < (int) segmentation.size(); i++) {
		std::vector<cv::Point> &coord= segmentation[i].coordinates;
		for (int j = 0; j < (int)coord.size(); ++j) {
			cv::Vec3b the_index;
			the_index[0] = i / (256*256);
			the_index[1] = (i % (256*256)) / 256;
			the_index[2] = i % 256;
			segmentation_index[coord[j].y][coord[j].x] = the_index;
		}
	}
#else
	// get global estimate
	#ifdef WITH_QT
	QTime t;
	t.start();
	#endif

	if( config.verbosity > 0 ){
		std::cout << "Starting patches-segmentation with segmentation method \"" << config.seg_method << "\"." << std::endl;
	}

	// create mask for excluding saturated and too dark pixels:
	cv::Mat_< unsigned char > mask_saturated_dark;
	cv::Mat_< cv::Vec3d > preprocessed_image;
	mask_saturated_dark = cv::Mat_< unsigned char >::zeros( normalized_image.rows, normalized_image.cols );
	estimator->preprocessImage(normalized_image, mask_saturated_dark, preprocessed_image, mask_saturated_dark );

	// segmentation
	std::vector< superpixels::Superpixel > segmentation;
	superpixels::PatchesSegmentation segP( config.seg_patches, &mask_saturated_dark, config.excluded_area );

	if( config.seg_method.compare( "felzenszwalb" ) == 0 ){
		superpixels::FelzenszwalbSegmentation* segF = new superpixels::FelzenszwalbSegmentation(config.seg_felz);
		segP.setSegmenter( segF );
		// TODO/FIXME: depending on whether we place config.setRandomSeed() before or after the next line
		//             we get a different value compared to our old code when placed before and the same
		//             when placed after. But within useSegmenter(), there is no call of srand() or rand()!?
		//             Maybe some OpenCV stuff called in the deeper calling hierarchy of useSegmenter()
		//             is using rand() somewhere?
		segmentation = segP.useSegmenter( preprocessed_image );
	}
	else if( config.seg_method.compare( "veksler" ) == 0 ){
		superpixels::VekslerSegmentation* segV = new superpixels::VekslerSegmentation(config.seg_veks);
		segP.setSegmenter( segV );
		// TODO/FIXME: depending on whether we place config.setRandomSeed() before or after the next line
		//             we get a different value compared to our old code when placed before and the same
		//             when placed after. But within useSegmenter(), there is no call of srand() or rand()!?
		//             Maybe some OpenCV stuff called in the deeper calling hierarchy of useSegmenter()
		//             is using rand() somewhere?
		segmentation = segP.useSegmenter( preprocessed_image );
	}
	else {
		std::cerr << "Error in int CommandIebv::execute(): "
			  << "unknown parameter for seg_method. Exiting." << std::endl;
		exit(1);
	}
	if( config.verbosity > 0 ){
		std::cout << "Number of segments detected: " << segmentation.size() << std::endl;
	}

	// set general random seed, e.g. for drawing patches randomly
	// TODO / FIXME: when I accidentally forgot to use setRandomSeed(), the first patch had a negative
	//               value in his estimate! Shouldn't that be impossible? Detailed patch info copied from commandline:
//	Patch 1: x = 274, y = 272 (width: 10, height: 40, num_pixels: 400)
//	        min slope = 1 > config.max_slope = 10 || max_slope = 2.34807 < config.min_slope 0.003
//	    1.1404 = 0.199655 - (0.920038 / -0.391828) * 0.400648
//	    0 = 0.400648 - (-0.707107 / -0.707107) * 0.400648
//	    -0.163466 = 0.399698 - (-0.814835 / -0.579693) * 0.400648
//	estimate:       [-0.163466, 0, 1.1404]
//	        min. num. pixels:: 1, eigenvector slope: 1, eccentricity: 1
	config.setRandomSeed();

	int number_tries_left = config.seg_patches.number_tries;
	std::vector< illumestimators::Illum > estimates;
	superpixels::Superpixel patch;
	while( ((int)estimates.size() < config.number_superpixels) && (number_tries_left--) )
	{
		#define GPR_VERSION 1
		#if GPR_VERSION==1
		segP.getPatchRandomly( preprocessed_image.rows, preprocessed_image.cols, patch );
		#elif GPR_VERSION==2
		segP.setImage( preprocessed_image );
		patch = segP.getPatchRandomly();
		#else
		segP.getPatchRandomly( preprocessed_image, patch );
		#endif

		int patch_no = config.seg_patches.number_tries - number_tries_left;

		if( config.verbosity > 0 ){
			std::cout << "Patch " << patch_no <<": x = " << patch.bbox.x << ", y = " << patch.bbox.y << " (width: " << patch.bbox.width
				  << ", height: " << patch.bbox.height << ", num_pixels: " << patch.coordinates.size() << ")" << std::endl << "        ";
		}

		illumestimators::Illum est = estimator->estimateIlluminant( preprocessed_image, patch );

		if( est.isValid ){
			estimates.push_back( est );
			if( config.verbosity > 0 ) std::cout << "        "; // for nicer alignment of output
		}

		// output
		if( config.verbosity > 0 ){
			int error = estimator->error();
			std::cout << "min. num. pixels:: "     << ((error & 1) ? "0, " : "1, ");
			std::cout << "eigenvector slope: "   << ((error & 2) ? "0, " : "1, ");
			std::cout << "eccentricity: " << ((error & 4) ? "0 "  : "1 " );
			std::cout << std::endl;
		}

		// DEBUG
//		if( patch_no == 5 ) break;
//		if( patch_no == 5 ) exit(1);
		// DEBUG END
	}

	if( config.verbosity > 0 ){
		std::cout << "number of patches used for voting: " << estimates.size() << std::endl;
	} else {
		std::cout << estimates.size() << " ";
	}

	// perform histogram voting
	illumestimators::HistogramVotingFusion histogram_voting( config.config_histogram_voting_fusion );
	estimate_ = histogram_voting.estimateIlluminant( estimates );

	// output
	if( config.verbosity > 0 ){
		#ifdef WITH_QT
		int elapsed = t.elapsed();
		std::cout << "time for whole estimation: " << elapsed / 1000 << "secs " << elapsed % 1000 << "ms" << std::endl;
		#endif
		std::cout << "Global estimate: [" << estimate_.r << "," << estimate_.g << "," << estimate_.b << "]" << std::endl;
	} else {
		std::cout << estimate_.r << " " << estimate_.g << " " << estimate_.b << std::endl;
	}

	// DEBUG OUTPUT for class Plotter
//	Plotter plotter( config );
//	plotter.registerMatrix( preprocessed_image, "preprocessed" );
//	plotter.registerMatrix( normalized_image, "normalized" );
	// DEBUG OUTPUT END

	return 0;
#endif

	std::string img_name = config.input_file_16_bit;
	if (config.input_file_16_bit.find_last_of('/') != std::string::npos) {
		img_name = img_name.substr(config.input_file_16_bit.find_last_of('/')+1);
	}
	if (img_name.find_last_of('.') != std::string::npos) {
		img_name = img_name.substr(0, img_name.find_last_of('.'));
	}

#if NEW_IEBV
	// TODO: write somewhere else then /tmp, i.e. -O option
	{ std::stringstream s; s << "/tmp/iic_iebv_tan_" << img_name << "_" << config.input_intensity_max << "_conf.png"; outfile = s.str(); }
	std::cout << "writing" << outfile << std::endl;
	cv::imwrite(outfile, confidence);


	// TODO: write somewhere else then /tmp, i.e. -O option
	{ std::stringstream s; s << "/tmp/iic_iebv_tan_" << img_name << "_" << config.input_intensity_max << "_est.png"; outfile = s.str(); }
	std::cout << "writing" << outfile << std::endl;
	cv::imwrite(outfile, color_segmented);
	{
		std::stringstream s;
		if (config.input_file.length() > 1) {
			// TODO: write somewhere else then /tmp, i.e. -O option
			s << "/tmp/seg_idx_" << img_name << ".png";
		} else {
			// TODO: write somewhere else then /tmp, i.e. -O option
			s << "/tmp/seg_idx_" << img_name << ".png";
		}
		outfile = s.str();
	}
	cv::imwrite(outfile, segmentation_index);
	std::cout << "wrote index file " << outfile << std::endl;
#endif // NEW_IEBV

	if( config.verbosity > 0 ){
		std::cout << "statistics:" << std::endl;
		std::cout << "number of segments: " << segmentation.size() << std::endl;
		std::cout << "FAILED min number pixels: " << error_stat[0] << std::endl;
		std::cout << "FAILED slope out of bounds: " << error_stat[1] << std::endl;
		std::cout << "FAILED eccentricity too low: " << error_stat[2] << std::endl;
		std::cout << "FAILED too small estimates: " << error_stat[3] << std::endl;
		std::cout << "FAILED too large estimates: " << error_stat[4] << std::endl;
	}

	return 0;
}

void CommandIebv::printShortHelp() const {
	std::cout << "Illuminant Estimation by Voting (modularized version)" << std::endl;
}

void CommandIebv::printHelp() const {
	std::cout << "Illuminant Estimation by Voting (modularized version)" << std::endl;
}



}
