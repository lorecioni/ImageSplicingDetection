#ifndef IIC_MASK_H
#define IIC_MASK_H

#include <opencv2/imgproc/imgproc.hpp>

namespace iic {

typedef enum {
	AND,
	OR,
	SATURATE_ADD,
	NORMALIZED_MULTIPLY
} MASKING_OPERATION;

class Mask {
public:
	Mask();

	// full image mask overlay
	virtual void addMask(const Mask &mask, const MASKING_OPERATION op);

	// mask overlay - no bounding box checking
	virtual void addMask(const Mask &mask, const cv::Point pos, const MASKING_OPERATION op);

	virtual cv::Mat_<uchar> getMask() const = 0;

protected:
	cv::Mat_<uchar> mask;
	bool valid;

};

}
#endif // IIC_MASK_H
