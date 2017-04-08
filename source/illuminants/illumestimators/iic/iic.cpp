/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

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

