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

// bezier curve of degree n; terminate at i=0
template<typename T, unsigned int n, unsigned int i = n>
struct Bezier
{
	static T position(const T* k, double t)
	{
		// see http://en.wikipedia.org/wiki/B%C3%A9zier_curve#General_definition
		return k[i] * static_cast<typename T::value_type>(BernsteinPolynomial<n, i>::value(t)) + Bezier<T, n, i - 1>::position(k, t);
	}
};

template<typename T, unsigned int n>
struct Bezier<T, n, 0>
{
	static T position(const T* k, double t)
	{
		return k[0] * static_cast<typename T::value_type>(BernsteinPolynomial<n, 0>::value(t));
	}
};

template <typename T, unsigned int n>
BezierCurve<T,n>::BezierCurve(const ControlPoint control_points[n + 1])
{
	for (auto i = 0; i < n + 1; ++i)
		k[i] = control_points[i];
}

template <typename T, unsigned int n>
T BezierCurve<T,n>::position(double t) const
{
	return Bezier<T,n>::position(k, t);
}

template <typename T, unsigned int n, unsigned int m>
BezierSurface<T,n,m>::BezierSurface(const ControlPoint control_points[n + 1][m + 1])
{
	for (auto i = 0; i < n + 1; ++i)
		for (auto j = 0; j < m + 1; ++j)
			k[i][j] = control_points[i][j];
}

template <typename T, unsigned int n, unsigned int m>
T BezierSurface<T,n,m>::position(double u, double v) const
{
	ControlPoint kn[n + 1];

	// evaluation curves in direction m/v to obtain intermediate control points in direction n/u
	for (auto i = 0; i < n + 1; ++i)
		kn[i] = Bezier<T,m>::position(k[i], v);

	return Bezier<T,n>::position(kn, u);
}

template <typename T, unsigned int n>
std::ostream& operator<< (std::ostream& os, const T& k)
{
	os << "(";
	for (auto i = 0; i < k.length(); ++i) {
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
	for (auto i = 0; i < n + 1; ++i) {
		if (i)
			os << ", ";
		::operator<< <T, n>(os, bc.k[i]);
	}

	return os;
}

template <typename T, unsigned int n, unsigned int m>
std::ostream& operator<< (std::ostream& os, const BezierSurface<T,n,m>& bs)
{
	for (auto i = 0; i < n + 1; ++i) {
		if (i)
			os << "\n";

		for (auto j = 0; j < n + 1; ++j) {
			if (j)
				os << ", ";
			::operator<< <T, n>(os, bs.k[i][j]);
		}
	}

	return os;
}

#endif
