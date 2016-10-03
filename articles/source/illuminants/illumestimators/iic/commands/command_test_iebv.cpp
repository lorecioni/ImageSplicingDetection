#include <vector>
#include <iostream>

#include "command_test_iebv.h"
#include "iebv.h"

namespace iic {

CommandTestIebv::CommandTestIebv() 
 : vole::Command(
		"iebvTest",
		config,
		"Christian Riess",
		"christian.riess@informatik.uni-erlangen.de")
{

}

int CommandTestIebv::execute() {
	cv::Mat_<cv::Vec3d> normalized_image(1, 4);
	normalized_image[0][0][0] = 180.0/500;
	normalized_image[0][0][1] = normalized_image[0][0][2] = 180.0/500;
	
	normalized_image[0][1][0] = 202.0/450;
	normalized_image[0][1][1] = normalized_image[0][1][2] = 100.0/450;
	
	normalized_image[0][2][0] = 200.0/400;
	normalized_image[0][2][1] = normalized_image[0][2][2] = 115.0/400;

	normalized_image[0][3][0] = 175.0/350;
	normalized_image[0][3][1] = normalized_image[0][3][2] = 95.0/350;

	Iebv iebv(config);
	iebv.createIICSpace(normalized_image);
	std::vector<cv::Point> indices;
	indices.push_back(cv::Point(0, 0));
	indices.push_back(cv::Point(1, 0));
	indices.push_back(cv::Point(2, 0));
	indices.push_back(cv::Point(3, 0));
	// TODO / FIXME: next line is not working, currently. We use superpixels::Superpixel
	//               as second parameter now, not std::vector<cv::Point> anymore.
	//               Maybe delete this whole class/command, as it should be deprecated?
	illumestimators::Illum result = illumestimators::Illum(); // iebv.estimateIlluminant(normalized_image, indices);
	std::cout << "result: " << result.r << ", " << result.g << ", " << result.b << std::endl;
	return 0;
}

void CommandTestIebv::printShortHelp() const
{
	std::cout << "just testing; delete me." << std::endl;
}

void CommandTestIebv::printHelp() const
{
	std::cout << "just testing; delete me." << std::endl;
}

}

