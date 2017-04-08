/*	
	Copyright(c) 2012 Christian Riess <christian.riess@cs.fau.de>
	and Johannes Jordan <johannes.jordan@cs.fau.de>.

	This file may be licensed under the terms of of the GNU General Public
	License, version 3, as published by the Free Software Foundation. You can
	find it here: http://www.gnu.org/licenses/gpl.html
*/

#ifndef IREAD_IMG_READ_H
#define IREAD_IMG_READ_H

// note gehlers max: 3690, darklevel bei prefix IMG_ = 129.0

#include <opencv2/imgproc/imgproc.hpp>

namespace iread {

	class IRead {
		public:

		static cv::Mat_<cv::Vec3d> img_read(
			const std::string &file_name,
			const std::string &colorspace,
			const int maxInt = -1,
			const int verbosity = 0);

		static cv::Mat_<cv::Vec3d> img_read_dl(
			const std::string &file_name,
			const std::string &colorspace,
			const int darklevel,
			const int maxInt = -1,
			const int verbosity = 0);

		static cv::Mat_<cv::Vec3d> img_read_dl_max(
			const std::string &file_name,
			const std::string &colorspace,
			const int darklevel,
			int &maxInt,
			const int verbosity = 0);
		
		static int getMax(cv::Mat_<cv::Vec3b> &mat);
		static int getMax(cv::Mat_<cv::Vec3w> &mat);
		static int getMax(cv::Mat_<cv::Vec3d> &mat);
	};

}

#endif // IREAD_IMG_READ_H
