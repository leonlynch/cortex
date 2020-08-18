/**
 * @file bezier.h
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_BEZIER_H
#define CORTEX_BEZIER_H

#include <cstddef>

#include <vector>
#include <ostream>

template <typename T, std::size_t n>
struct BezierCurve
{
	using ControlPoint = T;

	ControlPoint k[n + 1];

	BezierCurve() {}
	BezierCurve(const ControlPoint control_points[n + 1]);

	T position(double t) const;
	T normal(double t) const;

	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(std::size_t t_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};

template <typename T, std::size_t n, std::size_t m>
struct BezierSurface
{
	using ControlPoint = T;

	ControlPoint k[n + 1][m + 1];

	BezierSurface() {}
	BezierSurface(const ControlPoint control_points[n + 1][m + 1]);

	T position(double u, double v) const;
	T normal(double u, double v) const;

	template<typename VertexType, typename IndexType = unsigned int>
	void tesselate(std::size_t u_count, std::size_t v_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
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
