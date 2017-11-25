/**
 * @file bezier.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef __CORTEX_BEZIER_H__
#define __CORTEX_BEZIER_H__

#include <cstddef>

#include <vector>
#include <ostream>

template <typename T, std::size_t n>
struct BezierCurve
{
	typedef T ControlPoint;

	ControlPoint k[n + 1];

	BezierCurve() {}
	BezierCurve(const ControlPoint control_points[n + 1]);

	T position(double t) const;
	T normal(double t) const;

	struct Vertex
	{
		T position;
		T normal;
	};
	void tesselate(std::size_t t_count, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const;
};

template <typename T, std::size_t n, std::size_t m>
struct BezierSurface
{
	typedef T ControlPoint;

	ControlPoint k[n + 1][m + 1];

	BezierSurface() {}
	BezierSurface(const ControlPoint control_points[n + 1][m + 1]);

	T position(double u, double v) const;
	T normal(double u, double v) const;

	struct Vertex
	{
		T position;
		T normal;
	};
	void tesselate(std::size_t u_count, std::size_t v_count, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) const;
};

template <typename T, std::size_t n>
std::ostream& operator<< (std::ostream& os, const typename BezierCurve<T,n>::ControlPoint& k);

template <typename T, std::size_t n>
std::ostream& operator<< (std::ostream& os, const BezierCurve<T,n>& bc);

template <typename T, std::size_t n, std::size_t m>
std::ostream& operator<< (std::ostream& os, const typename BezierSurface<T,n,m>::ControlPoint& k);

template <typename T, std::size_t n, std::size_t m>
std::ostream& operator<< (std::ostream& os, const BezierSurface<T,n,m>& bs);


#include "bezier.tcc"

#endif
