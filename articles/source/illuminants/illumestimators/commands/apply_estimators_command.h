#ifndef ILLUMESTIMATORS_APPLY_ESTIMATORS_COMMAND_H
#define ILLUMESTIMATORS_APPLY_ESTIMATORS_COMMAND_H

#include "command.h"
#include "apply_estimators_config.h"
#include "superpixelsegmentation.h"
#include <opencv2/imgproc/imgproc.hpp>

namespace illumestimators {

class CommandApplyEstimators : public vole::Command
{
public:
	CommandApplyEstimators();
	int execute();

	void printShortHelp() const;
	void printHelp() const;

	ApplyEstimatorsConfig config;

	bool read_normalized_16_bit(std::string filename, cv::Mat_<cv::Vec3d> &img, int &minVal, int &maxVal);

	void getSuperpixelGroundTruth(
		cv::Mat_<cv::Vec3d> &ground_truth, std::vector<superpixels::Superpixel> &segmentation,
		std::vector<cv::Vec3d> &gt_values, cv::Mat_<cv::Vec3d> &sp_gt_image);

	cv::Mat_<double> completeGwShida(int derivative);
	cv::Mat_<double> completeGamutShida();
	cv::Mat_<double> completeBayesShida();

protected:
	void compute_illuminant_error(std::vector<cv::Vec3d> &illuminants, std::vector<cv::Vec3d> &gt_values, cv::Mat_<double> &errors);

private:


};


}

#endif // ILLUMESTIMATORS_APPLY_ESTIMATORS_COMMAND_H
