/**
 * @file bezier.tcc
 *
 * Copyright (c) 2013 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#include "bezier.h"

#include <cmath>

#include <utility>

#ifndef CORTEX_BEZIER_TCC
#define CORTEX_BEZIER_TCC

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
template<typename T, std::size_t n, std::size_t i = n>
struct Bezier
{
	static T position(const T* k, double t)
	{
		// see http://en.wikipedia.org/wiki/B%C3%A9zier_curve#General_definition
		return k[i] * static_cast<typename T::value_type>(BernsteinPolynomial<n, i>::value(t)) + Bezier<T, n, i - 1>::position(k, t);
	}

	static T tangent(const T* k, double t)
	{
		// bezier curve of degree n with control points k has derivative which is bezier curve of degree n-1 with control points d[i] = n(k[i + 1] - k[i])
		T d[n];
		for (std::size_t ni = 0; ni < n; ++ni)
			d[ni] = static_cast<typename T::value_type>(n) * (k[ni + 1] - k[ni]);

		return Bezier<T,n-1>::position(d, t);
	}
};

template<typename T, std::size_t n>
struct Bezier<T, n, 0>
{
	static T position(const T* k, double t)
	{
		return k[0] * static_cast<typename T::value_type>(BernsteinPolynomial<n, 0>::value(t));
	}
};

template <typename T, std::size_t n>
BezierCurve<T,n>::BezierCurve(const ControlPoint control_points[n + 1])
{
	for (std::size_t i = 0; i < n + 1; ++i)
		k[i] = control_points[i];
}

template <typename T, std::size_t n>
T BezierCurve<T,n>::position(double t) const
{
	return Bezier<T,n>::position(k, t);
}

template <typename T, std::size_t n>
T BezierCurve<T,n>::normal(double t) const
{
	T dt = Bezier<T,n>::tangent(k, t);

	// rotate 90deg counter-clockwise
	return T(-dt[1], dt[0]);
}

template <typename T, std::size_t n>
template <typename VertexType, typename IndexType>
void BezierCurve<T,n>::tesselate(std::size_t t_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	vertices.reserve(vertices.size() + t_count);
	indices.reserve(indices.size() + ((t_count - 1) * 2));

	std::size_t offset = vertices.size();
	for (std::size_t i = 0; i < t_count; ++i) {
		double t = i / static_cast<double>(t_count - 1);

		VertexType vertex;
		vertex.position = position(t);
		vertex.normal = normal(t);
		vertices.push_back(std::move(vertex));

		if (i < t_count - 1) {
			indices.push_back(offset + i);
			indices.push_back(offset + (i + 1));
		}
	}
}

template <typename T, std::size_t n, std::size_t m>
BezierSurface<T,n,m>::BezierSurface(const ControlPoint control_points[n + 1][m + 1])
{
	for (std::size_t i = 0; i < n + 1; ++i)
		for (std::size_t j = 0; j < m + 1; ++j)
			k[i][j] = control_points[i][j];
}

template <typename T, std::size_t n, std::size_t m>
T BezierSurface<T,n,m>::position(double u, double v) const
{
	ControlPoint kn[n + 1];

	// evaluate curves in direction m/v to obtain intermediate control points in direction n/u
	for (std::size_t i = 0; i < n + 1; ++i)
		kn[i] = Bezier<T,m>::position(k[i], v);

	return Bezier<T,n>::position(kn, u);
}

template <typename T, std::size_t n, std::size_t m>
T BezierSurface<T,n,m>::normal(double u, double v) const
{
	ControlPoint kn[n + 1];
	ControlPoint km[m + 1];

	// evaluate curves in direction m/v to obtain intermediate control points in direction n/u
	for (std::size_t i = 0; i < n + 1; ++i)
		kn[i] = Bezier<T,m>::position(k[i], v);

	// evaluate curves in direction n/u to obtain intermediate control points in direction m/v
	for (std::size_t i = 0; i < m + 1; ++i) {
		// transpose control points
		ControlPoint kmn[n + 1];
		for (std::size_t j = 0; j < n + 1; ++j)
			kmn[j] = k[j][i];

		km[i] = Bezier<T,n>::position(kmn, u);
	}

	T du = Bezier<T, n>::tangent(kn, u);
	T dv = Bezier<T, m>::tangent(km, v);

	// cross product
	return T(
		du[1] * dv[2] - du[2] * dv[1],
		du[2] * dv[0] - du[0] * dv[2],
		du[0] * dv[1] - du[1] * dv[0]
	);
}

template <typename T, std::size_t n, std::size_t m>
template <typename VertexType, typename IndexType>
void BezierSurface<T,n,m>::tesselate(std::size_t u_count, std::size_t v_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const
{
	vertices.reserve(vertices.size() + (u_count * v_count));
	indices.reserve(indices.size() + ((u_count - 1) * (v_count - 1) * 3 * 2));

	std::size_t offset = vertices.size();
	for (std::size_t i = 0; i < u_count; ++i) {
		for (std::size_t j = 0; j < v_count; ++j) {
			double u = i / static_cast<double>(u_count - 1);
			double v = j / static_cast<double>(v_count - 1);

			VertexType vertex;
			vertex.position = position(u, v);
			vertex.normal = normal(u, v);
			vertices.push_back(std::move(vertex));

			if (i < u_count - 1 && j < v_count - 1) {
				indices.push_back(offset + (i * u_count + j));
				indices.push_back(offset + ((i + 1) * u_count + j));
				indices.push_back(offset + (i * u_count + j + 1));
				indices.push_back(offset + (i * u_count + j + 1));
				indices.push_back(offset + ((i + 1) * u_count + j));
				indices.push_back(offset + ((i + 1) * u_count + j + 1));
			}
		}
	}
}

template <typename T, std::size_t n>
std::ostream& operator<< (std::ostream& os, const typename BezierCurve<T,n>::ControlPoint& k)
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

template <typename T, std::size_t n>
std::ostream& operator<< (std::ostream& os, const BezierCurve<T,n>& bc)
{
	for (std::size_t i = 0; i < n + 1; ++i) {
		if (i)
			os << ", ";
		::operator<< <T, n>(os, bc.k[i]);
	}

	return os;
}

template <typename T, std::size_t n, std::size_t m>
std::ostream& operator<< (std::ostream& os, const typename BezierSurface<T,n,m>::ControlPoint& k)
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

template <typename T, std::size_t n, std::size_t m>
std::ostream& operator<< (std::ostream& os, const BezierSurface<T,n,m>& bs)
{
	for (std::size_t i = 0; i < n + 1; ++i) {
		if (i)
			os << "\n";

		for (std::size_t j = 0; j < n + 1; ++j) {
			if (j)
				os << ", ";
			::operator<< <T, n>(os, bs.k[i][j]);
		}
	}

	return os;
}

#endif
