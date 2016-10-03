#include "fusion/histogramVoting/histogramVotingFusion.h"
#include "common/mask.h"
#include <iostream>

namespace illumestimators {

HistogramVotingFusion::HistogramVotingFusion( const ConfigHistogramVotingFusion& config )
{
	this->config = config;
}

HistogramVotingFusion::~HistogramVotingFusion()
{
}

std::string HistogramVotingFusion::name() const
{
	return std::string( "HistogramVotingFusion" );
}

std::string HistogramVotingFusion::identifier() const
{
	return std::string( "histogramVotingFusion" );
}

Illum HistogramVotingFusion::estimateIlluminant(
	const cv::Mat_<cv::Vec3d>& image,
	const cv::Mat_<unsigned char>& mask ) const
{
	// TODO: implement function
	std::cerr << "Warning: Illum HistogramVotingFusion::estimateIlluminant( const cv::Mat_<cv::Vec3d>& image, const cv::Mat_<unsigned char>& mask) const is not implemented, yet." << std::endl;
	return Illum();
}

Illum HistogramVotingFusion::estimateIlluminant(
	const cv::Mat_< cv::Vec3d >& image,
	const std::vector< cv::Point >& superpixel,
	const cv::Mat_< unsigned char >& mask ) const
{
	// TODO: implement function
	std::cerr << "Warning: Illum HistogramVotingFusion::estimateIlluminant( const cv::Mat_<cv::Vec3d>& image, const std::vector<cv::Point>& superpixel, const cv::Mat_<unsigned char>& mask ) const ist not implemented, yet" << std::endl;
	return Illum();
}

Illum HistogramVotingFusion::estimateIlluminant( const std::vector< cv::Vec3d >& estimates, bool isRGB ) const
{
	std::vector< Illum > copy;

	for( unsigned int i = 0; i < estimates.size(); i++ ){
		// just always guarantee that illum will be initialized in BGR-order
		int r_index = 2, g_index = 1, b_index = 0;
		if( isRGB == true ){
			r_index = 0;
			b_index = 2;
		}
		Illum illum( estimates[i][b_index], estimates[i][g_index], estimates[i][r_index] );
		copy.push_back( illum );
	}

	// TODO: the parent class Fusion wants all estimateIlluminant()-functions to be const. That means that
	//       we cannot use non-const helper functions, for example to fill the histogram with the estimates.
	//       Currently, some member variables are declared mutable... Change interface or let them mutable.
	return estimateIlluminant( copy );
}

Illum HistogramVotingFusion::estimateIlluminant( const std::vector< Illum >& estimates ) const
{
	// original voting:
	// 1) per patch call registerVote(gamma[0], gamma[1], gamma[2])
	// 2) smoothVotes(1)
	// 3) getVotingResult(estimate[0], estimate[1], estimate[2]);

	// use estimates-vector to fill our histogram (parameters given with config)
	if( !fillHistogram( estimates ) ){
		if( config.verbosity > 0 ){
			std::cerr << "Error in Illum HistogramVotingFusion::estimateIlluminant( const std::vector< Illum >& estimates ) const: "
				  << "calling fillHistogram() returned false. Returning Illum()." << std::endl;
		}
		return Illum();
	}

	// smooth the histogram
	if( !meanSmoothing() ){
		if( config.verbosity > 0 ){
			std::cerr << "Error in Illum HistogramVotingFusion::estimateIlluminant( const std::vector< Illum >& estimates ) const: "
				  << "calling meanSmoothing() returned false. Using unsmoothed histogram." << std::endl;
		}
	}

	// get the voting result
	Illum estimate = getBestEstimate();

	return estimate;
}

void HistogramVotingFusion::preprocessImage(const cv::Mat_< cv::Vec3d >& image, const cv::Mat_< unsigned char >& mask, cv::Mat_< cv::Vec3d > &outputImage, cv::Mat_< unsigned char > &outputMask) const
{
	std::cout << "HistogramVotingFusion::preprocessImage(): not implemented, yet" << std::endl;

	// TODO: even necessary to do anything in here?
//	outputImage = image.clone();
//	if ((image.rows != mask.rows) || (image.cols != mask.cols)) {
//		std::cerr << "No mask!" << std::endl;
//		outputMask = cv::Mat_<unsigned char>::zeros(outputImage.rows, outputImage.cols);
//	} else {
//		outputMask = mask.clone();
//	}
//	illumestimators::Mask::maskSaturatedPixels(outputImage, outputMask, config.max_intensity);
//	illumestimators::Mask::maskDarkPixels(outputImage, outputMask, config.min_intensity);
}

/** Train illuminant estimator with labelled images.
 *  \param imageFiles Filenames of training images.
 *  \param colorspaces Colorspaces of training images.
 *  \param illuminants Illuminant chromaticities vectors of training images.
 *  \param maskFiles Filenames of mask images.
 */
bool HistogramVotingFusion::train(
	const std::vector<std::string>& imageFiles,
	const std::vector<std::string>& colorspaces,
	const std::vector<cv::Vec3d>& illuminants,
	const std::vector<std::string>& maskFiles)
{
	return true;
}

/* no machine learning involved - no saving of training data */
bool HistogramVotingFusion::save(const std::string&) const
{
	return true;
}

/* no machine learning involved - no loading of training data */
bool HistogramVotingFusion::load(const std::string&)
{
	return true;
}

////////////////////////////////////
//    Private helper functions    //
////////////////////////////////////
bool HistogramVotingFusion::fillHistogram( const std::vector< Illum >& estimates ) const
{
	int number_bins = (int)(((config.upper_bound - config.lower_bound) / config.binsize)) + 1;
	histogram = cv::Mat3i( 1, number_bins, cv::Vec<int, 3>( 0, 0, 0 ) );

	if( number_bins < 1 ){
		if( config.verbosity > 0 ){
			std::cerr << "Error in void HistogramVotingFusion::fillHistogram( const std::vector< Illum >& estimates ) const: "
				  << "number of bins is invalid (" << number_bins << "), caused by given parameters: config.upper_bound ("
				  << config.upper_bound << "), config.lower_bound (" << config.lower_bound << "), config.binsize (" << config.binsize << ")" << std::endl;
		}
		return false;
	}

    std::cout << "HIST:" << std::endl;
	// for each estimate
	for( unsigned int i = 0; i < estimates.size(); i++ ){
		// for each of the three color channels, get the correct bin for the according estimate
		const Illum& illum = estimates[ i ];
		if( !illum.isValid ){
			if( config.verbosity > 0 ){
				std::cerr << "Warning in void HistogramVotingFusion::fillHistogram( const std::vector< Illum >& estimates ) const: "
					  << "illum number " << i+1 << " in given estimates-vector is not valid. Skipping this one for histogram evaluation." << std::endl;
			}
			continue;
		}
//        if (i % 10 == 0) std::cout << "[" << i << "]" << std::endl;
 //       std::cout << illum[2] << std::endl;


		// TODO: in original voting_histogram.cxx::addEntry(..) there were tests if current value is
		//       < lowerBound or > upperBound and member variables links-/rechts_rausgelaufen were
		//       set, but I think never used, later on. What was the intention and is that still
		//       necessary? I will clip values > upper_bound and < lower_bound to max and min, for now.
		// iterating over the channels should be fine since Illum always gives us the "correct" value,
		// depending on whether isBgr is set to true or false.
		for( int c = 0; c < histogram.channels(); c++ ){
			double value = illum[ c ];
			if( value < config.lower_bound ) value = config.lower_bound;
			if( value > config.upper_bound ) value = config.upper_bound;
			int bin = (value - config.lower_bound) / config.binsize;
			// TODO: in old code there was a parameter called weight so votes could be weithed differently.
			//       Necessary?
			histogram[0][ bin ][c]++ ;

		}
	}
  
	return true;
}

bool HistogramVotingFusion::meanSmoothing() const
{
	if( histogram.empty() ){
		if( config.verbosity > 0 ){
			std::cerr << "Error in bool HistogramVotingFusion::meanSmoothing() const: "
				  << "histogram has not been filled using fillHistogram()." << std::endl;
		}
		return false;
	}

//	if( smoothed_histogram.empty() ){
    smoothed_histogram = cv::Mat3d( histogram.rows, histogram.cols, cv::Vec< double, 3 >( 0.0, 0.0, 0.0 ) );
//	}

	int half_size = std::floor( ((config.window_size + 1.0) / 2.0) + 0.5 ); // for config.window_size = 1 -> half_size = 1, for 2 -> 2, for 3 -> 2, ...

	for( int y = 0; y < histogram.rows; y++ ){
        // start with x = 1, i.e. ignore an estimate of 0 as they appear quite often and can ruin a real estimate, esp. with smoothing
		for( int x = 1; x < histogram.cols; x++ ){
			for( int c = 0; c < histogram.channels(); c++ ){
				// calculate current window
				double value   = 0.0;
				int normalizer = 0;
				for( int w = -half_size; w <= half_size; w++ ){
					int index = x + w; // current index = position in histogram + window index
//					if( (index >= 0) && (index < histogram.cols ) ){
//						value += histogram[y][index][c];
//						normalizer++;
//                  }

                    // special treating of border values
                    if( index <= 0 ) index = 2-index;
                    if( index >= histogram.cols ){
                        int mirror_val = index - histogram.cols + 1;
                        index = index - mirror_val;
                    }

					value += histogram[y][index][c];
					normalizer++;
                }
				smoothed_histogram[y][x][c] = (normalizer > 0 ? value / normalizer : 0.0);
			}
		}
	}

//    for( int c = 0; c < smoothed_histogram.channels(); c++ ){
//        std::cout << "hist for (smoothed) channel " << c << ": " << std::endl;
//        for( int i = 0; i < smoothed_histogram.cols; i ++ ){
//            if (i % 10 == 0) std::cout << " [" << i << "] ";
//               std::cout << smoothed_histogram[0][i][c] << std::endl;
//        }
//    }



	return true;
}

Illum HistogramVotingFusion::getBestEstimate( int histogram_index ) const
{
	Illum estimate;

	cv::Vec3i best_bins;

	if( !smoothed_histogram.empty() ){
		best_bins = getBestBins( smoothed_histogram, histogram_index );
	} else {
		best_bins = getBestBins( histogram, histogram_index );
	}

	estimate.b = (best_bins[ 0 ] * config.binsize) + config.lower_bound;
	estimate.g = (best_bins[ 1 ] * config.binsize) + config.lower_bound;
	estimate.r = (best_bins[ 2 ] * config.binsize) + config.lower_bound;
	estimate.isValid = true;

	// normalize so the three resulting channels add up to 1, necessary as partitioning into
	// bins for voting might introduce rounding errors that lead to sums != 1
	estimate.normalize();

	return estimate;
}

cv::Vec3i HistogramVotingFusion::getBestBins( const cv::Mat3d& histogram, int histogram_index ) const
{
	cv::Vec3i best_bins( -1, -1, -1 );

	if( config.force_relation ){
		int max_vote = INT_MIN;
		// we select the first two values, e.g. R and G, and the third value is hence pre-defined
		// as they must add up to 1, i.e. to the number of bins = histogram.cols
		for( int i = 0; i < histogram.cols; i++ ){ // first channel
			for( int j = 0; j < histogram.cols; j++ ){ // second channel
				int k = histogram.cols - (i + j); // third channel as a consequence
				int vote = histogram[ histogram_index ][ i ][ 0 ] + histogram[ histogram_index ][ j ][ 1 ] + histogram[ histogram_index ][ k ][ 2 ];
				if( vote > max_vote ){
					max_vote = vote;
					best_bins[ 0 ] = i;
					best_bins[ 1 ] = j;
					best_bins[ 2 ] = k;
				}
			}
		}
	} else {
		// for each color channel, we need to check for the maximal voting and set the cv::Vec3i accordingly
		double max_votes[3] = {DBL_MIN, DBL_MIN, DBL_MIN}; // max votes for each channel
		for( int i = 0; i < histogram.cols; i ++ ){
			for( int c = 0; c < histogram.channels(); c++ ){
				if( histogram[ histogram_index ][ i ][ c ] > max_votes[ c ] ){
					max_votes[ c ] = histogram[ histogram_index ][ i ][ c ];
					best_bins[ c ] = i;
				}
			}
		}
	}

	return best_bins;
}

int HistogramVotingFusion::error()
{
	return 0;
}


} // namespace
