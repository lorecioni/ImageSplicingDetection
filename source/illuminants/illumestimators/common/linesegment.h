#ifndef ILLUMESTIMATORS_COMMON_LINESEGMENT_H
#define ILLUMESTIMATORS_COMMON_LINESEGMENT_H

#include <opencv2/core/core.hpp>

namespace illumestimators {

/** The LineSegment class provides calculation of intersections of line segments. */
class LineSegment
{
public:
	enum IntersectResult {PARALLEL, COINCIDENT, INTERESECTING, NOT_INTERESECTING};

public:
	/** Constructs an line segment with endpoints a and b. */
	LineSegment(const cv::Point2f& a, const cv::Point2f& b);
	~LineSegment();

public:
	/** Get first endpoint of the line segment. */
	cv::Point2f a() const { return m_a; }
	/** Get second endpoint of the line segment. */
	cv::Point2f b() const { return m_b; }

public:
	/** Calculate the intersection between two line segments. */
	IntersectResult intersect(const LineSegment& l, std::vector<cv::Point2f>& intersectionPoints) const;

private:
	std::vector<cv::Point2f> resultHelper(const cv::Point2f& a, const cv::Point2f& b) const;
	bool checkColinearPoint(const cv::Point2f& p) const;

private:
	/** First endpoint of the line segment. */
	cv::Point2f m_a;
	/** Second endpoint of the line segment. */
	cv::Point2f m_b;
};

} // illumestimators

#endif // ILLUMESTIMATORS_COMMON_LINESEGMENT_H
