#ifndef LIB_PROCESSING_CONVEXHULL_H
#define LIB_PROCESSING_CONVEXHULL_H

#include <opencv2/core/core.hpp>
#include <vector>
#include <iostream>
extern "C" {
#include <qhull/qhull_a.h>
}

/** Representation of a facet of a convex hull. */
template<int dim>
struct Facet {
	Facet(cv::Vec<double, dim> normal, double offset) {
		this->normal = normal;
		this->offset = offset;
	}

	/** Normal vector of the facet. */
	cv::Vec<double, dim> normal;
	/** Offset of the facet from the origin. */
	double offset;
};

/** The ConvexHull class provides calculation of convex hulls. */
template<int dim>
class ConvexHull
{
public:
	/** Constructs an empty convex hull. */
	ConvexHull();
	/** Constructs the convex hull from a set of points. */
	ConvexHull(const std::vector<cv::Vec<double, dim> >& points);
	~ConvexHull();

public:
	/** Add a point to the convex hull. */
	void addPoint(const cv::Vec<double, dim>& point);
	/** Add a set of points to the convex hull. */
	void addPoints(const std::vector<cv::Vec<double, dim> >& points);
	/** Set the points of the convex hull. */
	void setPoints(const std::vector<cv::Vec<double, dim> >& points);

public:
	/** Get the facets of the convex hull. */
	std::vector<Facet<dim> > facets();
	/** Get the vertices of the convex hull. */
	std::vector<cv::Vec<double, dim> > vertices();

public:
	/** Clear the points of the convex hull. */
	void clear();
	/** Recalculate vertices and facets of the convex hull. */
	void update();

private:
	bool m_update;
	std::vector<Facet<dim> > m_facets;
	std::vector<cv::Vec<double, dim> > m_vertices;
};

template <int dim>
ConvexHull<dim>::ConvexHull() :
	m_update(false)
{

}

template <int dim>
ConvexHull<dim>::ConvexHull(const std::vector<cv::Vec<double, dim> >& points) :
	m_update(false)
{
	setPoints(points);
}

template <int dim>
ConvexHull<dim>::~ConvexHull()
{

}

template <int dim>
void ConvexHull<dim>::addPoint(const cv::Vec<double, dim>& point)
{
	m_vertices.push_back(point);

	m_update = true;
}

template <int dim>
void ConvexHull<dim>::addPoints(const std::vector<cv::Vec<double, dim> >& points)
{
	m_vertices.insert(m_vertices.end(), points.begin(), points.end());

	m_update = true;
}

template <int dim>
void ConvexHull<dim>::setPoints(const std::vector<cv::Vec<double, dim> >& points)
{
	m_vertices = points;

	m_update = true;
}

template <int dim>
std::vector<Facet<dim> > ConvexHull<dim>::facets()
{
	if (m_update) {
		update();
	}

	return m_facets;
}

template <int dim>
std::vector<cv::Vec<double, dim> > ConvexHull<dim>::vertices()
{
	if (m_update) {
		update();
	}

	return m_vertices;
}

template <int dim>
void ConvexHull<dim>::clear()
{
	m_vertices.clear();
	m_facets.clear();

	m_update = false;
}

template <int dim>
void ConvexHull<dim>::update()
{
	if (!m_update) {
		return;
	}

	const unsigned int number = m_vertices.size();
	coordT* points = new coordT[dim * number];

	if (!points) {
		std::cerr << "Convex hull failed to allocate memory!" << std::endl;
		return;
	}

	for (unsigned int n = 0; n < number; n++) {
		for (int i = 0; i < dim; i++) {
			points[n * dim + i] = m_vertices[n][i];
		}
	}

	m_vertices.clear();
	m_facets.clear();

	char flags[] = "qhull Qt";

	if (!qh_new_qhull (dim, number, points, false, flags, 0/*stdout*/, stderr)) {
		facetT* facet;

		FORALLfacets {
			cv::Vec<double, dim> normal;

			for (int i = 0; i < dim; i++) {
				normal[i] = facet->normal[i];
			}

			Facet<dim> f(normal, facet->offset);

			m_facets.push_back(f);
		}

		vertexT* vertex;

		FORALLvertices {
			cv::Vec<double, dim> v;

			for (int i = 0; i < dim; i++) {
				v[i] = vertex->point[i];
			}

			m_vertices.push_back(v);
		}
	}

	qh_freeqhull(!qh_ALL);

	int curlong;
	int totlong;
	qh_memfreeshort(&curlong, &totlong);

	if (curlong || totlong) {
		std::cerr << "Qhull failed to free memory!" << std::endl;
	}

	delete [] points;

	m_update = false;
}

#endif // LIB_PROCESSING_CONVEXHULL_H
