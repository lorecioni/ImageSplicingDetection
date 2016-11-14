#include "convexpolygon.h"
#include "linesegment.h"
#include <iostream>

namespace illumestimators {

ConvexPolygon::ConvexPolygon()
{

}

ConvexPolygon::ConvexPolygon(const std::vector<cv::Point2f>& vertices) :
	m_vertices(vertices)
{

}

ConvexPolygon::~ConvexPolygon()
{

}

ConvexPolygon ConvexPolygon::intersect(const ConvexPolygon& poly) const
{
	ConvexPolygon result;

	int a = 0;
	int b = 0;

	int aa = 0;
	int ba = 0;

	enum State {IN_A, IN_B, UNKNOWN};
	State state = UNKNOWN;

	bool firstPoint = true;

	do {
		const int a1 = (a + m_vertices.size() - 1) % m_vertices.size();
		const int b1 = (b + poly.vertices().size() - 1) % poly.vertices().size();

		const cv::Point2f vecA = m_vertices[a] - m_vertices[a1];
		const cv::Point2f vecB = poly.vertices()[b] - poly.vertices()[b1];

		const float cross = signedArea2(cv::Point2f(0, 0), vecA, vecB);
		const float aHB = signedArea2(poly.vertices()[b1], poly.vertices()[b], m_vertices[a]);
		const float bHA = signedArea2(m_vertices[a1], m_vertices[a], poly.vertices()[b]);

		//std::cout << "cross=" << cross << ", aHB=" << aHB << ", bHA=" << bHA << std::endl;

		std::vector<cv::Point2f> intersectionPoints;
		LineSegment::IntersectResult intersectResult = LineSegment(m_vertices[a1], m_vertices[a]).intersect(LineSegment(poly.vertices()[b1], poly.vertices()[b]), intersectionPoints);

		if (intersectResult == LineSegment::INTERESECTING) {
			if (firstPoint && state == UNKNOWN) {
				aa = 0;
				ba = 0;
				firstPoint = false;
			}

			if (aHB > 0) {
				state = IN_A;
			} else if (bHA > 0) {
				state = IN_B;
			}

			assert(intersectionPoints.size() > 0);
			result.addPoint(intersectionPoints[0]);
		}

		if ((intersectResult == LineSegment::COINCIDENT) && (intersectionPoints.size() > 0) && (vecA.dot(vecB) < 0)) {
			result = ConvexPolygon(intersectionPoints);
			return result;
		}

		if ((std::abs(cross) <= std::numeric_limits<float>::epsilon()) && (std::abs(aHB) <= std::numeric_limits<float>::epsilon()) && (std::abs(bHA) <= std::numeric_limits<float>::epsilon())) {
			if (state == IN_A) {
				ba++;
				b = (b + 1) % poly.vertices().size();
			} else {
				aa++;
				a = (a + 1) % m_vertices.size();
			}
		} else if ((std::abs(cross) <= std::numeric_limits<float>::epsilon()) && (aHB < 0) && (bHA < 0)) {
			std::cout << "III" << std::endl;
			result.clear();
			return result;
		} else if (cross >= 0) {
			if (bHA > 0) {
				if (state == IN_A) {
					result.addPoint(m_vertices[a]);
				}
				aa++;
				a = (a + 1) % m_vertices.size();
			} else {
				if (state == IN_B) {
					result.addPoint(poly.vertices()[b]);
				}
				ba++;
				b = (b + 1) % poly.vertices().size();
			}
		} else {
			if (aHB > 0) {
				if (state == IN_B) {
					result.addPoint(poly.vertices()[b]);
				}
				ba++;
				b = (b + 1) % poly.vertices().size();
			} else {
				if (state == IN_A) {
					result.addPoint(m_vertices[a]);
				}
				aa++;
				a = (a + 1) % m_vertices.size();
			}
		}

		//std::cout << "a=" << a << ", b=" << b << ", aa=" << aa << ", ba=" << ba << ", state=" << state << std::endl;
	} while (((aa < (int)m_vertices.size()) || (ba < (int)poly.vertices().size())) && (aa < 2 * (int)m_vertices.size()) && (ba < 2 * (int)poly.vertices().size()));

	return result;
}

float ConvexPolygon::signedArea2(const cv::Point2f& a, const cv::Point2f& b, const cv::Point2f& c) const
{
	return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

void ConvexPolygon::addPoint(const cv::Point2f& p)
{
	if ((m_vertices.size() == 0) ||
		(((std::abs(m_vertices[m_vertices.size() - 1].x - p.x) > std::numeric_limits<float>::epsilon()) ||
		  (std::abs(m_vertices[m_vertices.size() - 1].y - p.y) > std::numeric_limits<float>::epsilon())) &&
		 ((std::abs(m_vertices[0].x - p.x) > std::numeric_limits<float>::epsilon()) ||
		 (std::abs(m_vertices[0].y - p.y) > std::numeric_limits<float>::epsilon())))
	) {
		m_vertices.push_back(p);
	}
}

void ConvexPolygon::print(const std::string& name) const
{
	std::cout << name << ":" << std::endl;

	for (unsigned int i = 0; i < m_vertices.size(); i++) {
		std::cout << m_vertices[i].x << " " << m_vertices[i].y << std::endl;
	}

	std::cout << std::endl;
}

void ConvexPolygon::test()
{
	ConvexPolygon polyA;
	ConvexPolygon polyB;

	polyA.addPoint(cv::Point2f(1.2647, 0.737144));
	polyA.addPoint(cv::Point2f(1.09702, 2.24956));
	polyA.addPoint(cv::Point2f(0.6286, 2.28678));
	polyA.addPoint(cv::Point2f(0.505286, 2.26628));
	polyA.addPoint(cv::Point2f(0.390494, 2.11702));
	polyA.addPoint(cv::Point2f(0.300014, 1.97472));
	polyA.addPoint(cv::Point2f(0.145461, 1.70306));
	polyA.addPoint(cv::Point2f(0.0566236, 0.907763));
	polyA.addPoint(cv::Point2f(0.0161411, 0.400278));
	polyA.addPoint(cv::Point2f(0.00799566, 0.209088));
	polyA.addPoint(cv::Point2f(0, 0));
	polyA.addPoint(cv::Point2f(0.669949, 0.375082));

	polyB.addPoint(cv::Point2f(1.45802, 0.24155));
	polyB.addPoint(cv::Point2f(1.2647, 0.737144));
	polyB.addPoint(cv::Point2f(0.724684, 0.74934));
	polyB.addPoint(cv::Point2f(0.582521, 0.742624));
	polyB.addPoint(cv::Point2f(0.450183, 0.693712));
	polyB.addPoint(cv::Point2f(0.345872, 0.647083));
	polyB.addPoint(cv::Point2f(0.167695, 0.558064));
	polyB.addPoint(cv::Point2f(0.0652787, 0.297459));
	polyB.addPoint(cv::Point2f(0.0186083, 0.131165));
	polyB.addPoint(cv::Point2f(0.00921783, 0.0685148));
	polyB.addPoint(cv::Point2f(0, 0));
	polyB.addPoint(cv::Point2f(0.772354, 0.122908));

	ConvexPolygon polyC = polyA.intersect(polyB);

	polyA.print("A");
	polyB.print("B");
	polyC.print("C");
}

} // illumestimators
