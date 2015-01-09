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

#include <ostream>

template <typename T, unsigned int n>
struct BezierCurve
{
	typedef T ControlPoint;

	ControlPoint k[n + 1];

	BezierCurve() {}
	BezierCurve(const ControlPoint control_points[n + 1]);
	T eval(double t) const;
};

template <typename T, unsigned int n, unsigned int m>
struct BezierSurface
{
	typedef T ControlPoint;

	BezierCurve<T,m> bc[n + 1];

	BezierSurface() {}
	BezierSurface(const ControlPoint control_points[n + 1][m + 1]);
	T eval(double u, double v) const;
};

template <typename T, unsigned int n>
std::ostream& operator<< (std::ostream& os, const T& k);

template <typename T, unsigned int n>
std::ostream& operator<< (std::ostream& os, const BezierCurve<T,n>& bc);

template <typename T, unsigned int n, unsigned int m>
std::ostream& operator<< (std::ostream& os, const BezierSurface<T,n,m>& bs);


#include "bezier.tcc"

#endif
