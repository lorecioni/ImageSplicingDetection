#include "linesegment.h"
#include <iostream>

namespace illumestimators {

LineSegment::LineSegment(const cv::Point2f& a, const cv::Point2f& b) :
	m_a(a),
	m_b(b)
{

}

LineSegment::~LineSegment()
{

}

LineSegment::IntersectResult LineSegment::intersect(const LineSegment& l, std::vector<cv::Point2f>& intersectionPoints) const
{
	const float denominator  = (l.b().y - l.a().y) * (m_b.x - m_a.x) - (l.b().x - l.a().x) * (m_b.y - m_a.y);
	const float numerator0 = (l.b().x - l.a().x) * (m_a.y - l.a().y) - (l.b().y - l.a().y) * (m_a.x - l.a().x);
	const float numerator1 = (m_b.x - m_a.x) * (m_a.y - l.a().y) - (m_b.y - m_a.y) * (m_a.x - l.a().x);

	const bool parallel = std::abs(denominator) < std::numeric_limits<float>::epsilon();
	const bool coincident = parallel && (std::abs(numerator0) < std::numeric_limits<float>::epsilon()) && (std::abs(numerator1) < std::numeric_limits<float>::epsilon());

	if (coincident) {
		if (checkColinearPoint(l.a()) &&  checkColinearPoint(l.b())) {
			intersectionPoints = resultHelper(l.a(), l.b());
			return COINCIDENT;
		}

		if (l.checkColinearPoint(m_a) &&  l.checkColinearPoint(m_b)) {
			intersectionPoints = resultHelper(m_a, m_b);
			return COINCIDENT;
		}

		if (checkColinearPoint(l.a()) &&  l.checkColinearPoint(m_a)) {
			intersectionPoints = resultHelper(l.a(), m_a);
			return COINCIDENT;
		}

		if (checkColinearPoint(l.a()) &&  l.checkColinearPoint(m_b)) {
			intersectionPoints = resultHelper(l.a(), m_b);
			return COINCIDENT;
		}

		if (checkColinearPoint(l.b()) &&  l.checkColinearPoint(m_a)) {
			intersectionPoints = resultHelper(l.b(), m_a);
			return COINCIDENT;
		}

		if (checkColinearPoint(l.b()) &&  l.checkColinearPoint(m_b)) {
			intersectionPoints = resultHelper(l.b(), m_b);
			return COINCIDENT;
		}

		return COINCIDENT;
	} else if (parallel) {
		return PARALLEL;
	}

	const float u0 = numerator0 / denominator;
	const float u1 = numerator1 / denominator;

	const bool intersect = (u0 >= 0) && (u0 <= 1) && (u1 >= 0) && (u1 <= 1);

	if (intersect) {
		cv::Point2f p;
		p.x = m_a.x + u0 * (m_b.x - m_a.x);
		p.y = m_a.y + u0 * (m_b.y - m_a.y);

		std::vector<cv::Point2f> points;
		points.push_back(cv::Point2f(0, 0));
		points.push_back(m_a);
		points.push_back(m_b);
		points.push_back(l.a());
		points.push_back(l.b());

		for (unsigned int i = 0; i < points.size(); i++) {
			if ((std::abs(p.x - points[i].x) <= std::numeric_limits<float>::epsilon()) && (std::abs(p.y - points[i].y) <= std::numeric_limits<float>::epsilon())) {
				p = points[i];
				break;
			}
		}

		intersectionPoints.push_back(p);

		return INTERESECTING;
	}

	return NOT_INTERESECTING;
}

bool LineSegment::checkColinearPoint(const cv::Point2f& p) const
{
	if (std::abs(m_a.x - m_b.x) > std::numeric_limits<float>::epsilon())
		return ((m_a.x <= p.x) && (p.x <= m_b.x)) || ((m_a.x >= p.x) && (p.x >= m_b.x));
	else
		return ((m_a.y <= p.y) && (p.y <= m_b.y)) || ((m_a.y >= p.y) && (p.y >= m_b.y));
}

std::vector<cv::Point2f> LineSegment::resultHelper(const cv::Point2f& a, const cv::Point2f& b) const
{
	std::vector<cv::Point2f> result;

	result.push_back(a);

	if (a != b) {
		result.push_back(b);
	}

	return result;
}

} // illumestimators
