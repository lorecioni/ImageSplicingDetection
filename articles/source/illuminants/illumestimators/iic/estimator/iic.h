#ifndef IIC_IIC_H
#define IIC_IIC_H

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

namespace iic {

class IIC {
public:
	IIC();
	IIC(cv::Mat_<cv::Vec3b> input);
	IIC(cv::Mat_<cv::Vec3s> input);
	IIC(cv::Mat_<cv::Vec3d> input);

	void setImage(cv::Mat_<cv::Vec3b> input);
	void setImage(cv::Mat_<cv::Vec3s> input);
	void setImage(cv::Mat_<cv::Vec3d> input);

	bool isValid();
	cv::Mat_<cv::Vec4d> getIIC();
	
private:

	template <typename T>
	void createIIC(cv::Mat_<T> input) { // for bytes or shorts
		iic_space = cv::Mat_<cv::Vec4d>(input.rows, input.cols);
//		bool out_of_bounds;
//		//debug:
//		int ctr = 0;
//		int ctr1 = 0;
//		int ctr2 = 0;
		for (int y = 0; y < input.rows; ++y) {
			for (int x = 0; x < input.cols; ++x) {
				// DEBUG OUTPUT
//				if( y >= 981 && y < 981+40 && x >= 232 && x < 232+10 ){
//					ctr1++;
//					std::cout << "[" << y << "][" << x << "]: " << input[y][x][0] + input[y][x][1] + input[y][x][2];

//					if( input[y][x][0] + input[y][x][1] + input[y][x][2] < 1.0 ){
//						std::cout << " -> KLEINER 1!" << std::endl;
//						ctr++;
//					} else {
//						std::cout << " -> OK" << std::endl;
//						ctr2++;
//					}
//				}
				// DEBUG OUTPUT END

				iic_space[y][x][3] = 1.0/(input[y][x][0] + input[y][x][1] + input[y][x][2]);

				for (int c = 0; c < 3; ++c) {
					iic_space[y][x][c] = input[y][x][c] * iic_space[y][x][3];
				}
			}
		}
		// DEBUG OUTPUT
//		std::cout << ctr << " mal kleiner 1 in diesem patch, " << ctr1 << " pixel total, " << ctr2 << " ok" << std::endl;
		// DEBUG OUTPUT END
		valid = true;
	}

	cv::Mat_<cv::Vec4d> iic_space;
	bool valid;
//	double min_intensity;
//	double max_intensity;
};

}
#endif // IIC_IIC_H
