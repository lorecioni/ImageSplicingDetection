#ifndef ILLUMESTIMATORS_COMMON_CONVEXPOLYGON_H
#define ILLUMESTIMATORS_COMMON_CONVEXPOLYGON_H

#include <opencv2/core/core.hpp>

namespace illumestimators {

/** The ConvexPolygon class provides calculation of intersections of convex polygon.
  * Algorithm adapted from Rourke, "Computational Geometry in C". Read chapter 7.
  */
class ConvexPolygon
{
public:
	/** Constructs an empty convex polygon. */
	ConvexPolygon();
	/** Constructs an convex polygon from vertices. */
	ConvexPolygon(const std::vector<cv::Point2f>& vertices);
	~ConvexPolygon();

public:
	/** Calculate the intersection between two convex polygons. */
	ConvexPolygon intersect(const ConvexPolygon& poly) const;

public:
	void addPoint(const cv::Point2f& p);
	void clear() { m_vertices.clear(); }

public:
	void print(const std::string& name) const;
	std::vector<cv::Point2f> vertices() const { return m_vertices; }

public:
	static void test();

private:
	float signedArea2(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c) const;

private:
	/** Vertices of the convex polygon. */
	std::vector<cv::Point2f> m_vertices;
};

} // illumestimators

#endif // ILLUMESTIMATORS_COMMON_CONVEXPOLYGON_H
