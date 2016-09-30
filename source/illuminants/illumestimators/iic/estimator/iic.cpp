#include <cassert>
#include <limits>

#include "iic.h"

namespace iic {

IIC::IIC()
{
	valid = false;
}

IIC::IIC(cv::Mat_<cv::Vec3d> input)
{
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3d>(input);
}

IIC::IIC(cv::Mat_<cv::Vec3b> input)
{
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3b>(input);
}

IIC::IIC(cv::Mat_<cv::Vec3s> input)
{
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3s>(input);
}

void IIC::setImage(cv::Mat_<cv::Vec3b> input) {
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3b>(input);
}

void IIC::setImage(cv::Mat_<cv::Vec3s> input) {
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3s>(input);
}

void IIC::setImage(cv::Mat_<cv::Vec3d> input) {
	assert((input.rows != 0) && (input.cols != 0));
	createIIC<cv::Vec3d>(input);
}

cv::Mat_<cv::Vec4d> IIC::getIIC() {
	assert(valid);
	return iic_space;
}

}

