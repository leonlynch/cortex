/**
 * @file bezier.tcc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "bezier.h"

#include <cstddef>
#include <cmath>

#ifndef __CORTEX_BEZIER_TCC__
#define __CORTEX_BEZIER_TCC__

// compile-time factorial template; terminate at x=0
template<unsigned int x>
struct Factorial
{
	static const unsigned int value = x * Factorial<x - 1>::value;
};

template<>
struct Factorial<0>
{
	static const unsigned int value = 1;
};

// binomial coefficient of degree n for term k
template<unsigned int n, unsigned int k>
struct BinomialCoefficient
{
	static const unsigned int value = Factorial<n>::value / (Factorial<k>::value * Factorial<n - k>::value);
};

// bernstein polynomial v of degree n for value x
template<unsigned int n, unsigned int v>
struct BernsteinPolynomial
{
	static double value(double x)
	{
		// see http://en.wikipedia.org/wiki/Bernstein_polynomial#Definition
		return BinomialCoefficient<n, v>::value * std::pow(x, v) * std::pow(1 - x, n - v);
	}
};

// bezier point evaluation of degree n for control point i; terminate at i=0
template<typename T, unsigned int n, unsigned int i>
struct BezierPoint
{
	static T eval(const T* k, double t)
	{
		// see http://en.wikipedia.org/wiki/B%C3%A9zier_curve#General_definition
		return k[i] * static_cast<typename T::value_type>(BernsteinPolynomial<n, i>::value(t)) + BezierPoint<T, n, i - 1>::eval(k, t);
	}
};

template<typename T, unsigned int n>
struct BezierPoint<T, n, 0>
{
	static T eval(const T* k, double t)
	{
		return k[0] * static_cast<typename T::value_type>(BernsteinPolynomial<n, 0>::value(t));
	}
};

template <typename T, unsigned int n>
BezierCurve<T,n>::BezierCurve(const ControlPoint control_points[n + 1])
{
	for (std::size_t i = 0; i < n + 1; ++i)
		k[i] = control_points[i];
}

template <typename T, unsigned int n>
T BezierCurve<T,n>::eval(double t) const
{
	return BezierPoint<T, n, n>::eval(k, t);
}

template <typename T, unsigned int n, unsigned int m>
BezierSurface<T,n,m>::BezierSurface(const ControlPoint control_points[n + 1][m + 1])
{
	for (std::size_t i = 0; i < n + 1; ++i)
		for (std::size_t j = 0; j < m + 1; ++j)
			bc[i].k[j] = control_points[i][j];
}

template <typename T, unsigned int n, unsigned int m>
T BezierSurface<T,n,m>::eval(double u, double v) const
{
	ControlPoint k[n + 1];

	// compute intermediate control points
	for (std::size_t i = 0; i < n + 1; ++i)
		k[i] = bc[i].eval(v);

	BezierCurve<T,n> tmp(k);
	return tmp.eval(u);
}

template <typename T, unsigned int n>
std::ostream& operator<< (std::ostream& os, const T& k)
{
	os << "(";
	for (std::size_t i = 0; i < k.length(); ++i) {
		if (i)
			os << ", ";
		os << k[i];
	}
	os << ")";

	return os;
}

template <typename T, unsigned int n>
std::ostream& operator<< (std::ostream& os, const BezierCurve<T,n>& bc)
{
	for (std::size_t i = 0; i < n + 1; ++i) {
		if (i)
			os << ", ";
		::operator<< <T, n>(os, bc.k[i]);
	}

	return os;
}

template <typename T, unsigned int n, unsigned int m>
std::ostream& operator<< (std::ostream& os, const BezierSurface<T,n,m>& bs)
{
	for (std::size_t i = 0; i < n + 1; ++i) {
		if (i)
			os << "\n";
		::operator<< <T, n>(os, bs.bc[i]);
	}

	return os;
}

#endif
