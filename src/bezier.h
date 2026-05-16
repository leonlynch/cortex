/**
 * @file bezier.h
 *
 * Copyright (c) 2013, 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_BEZIER_H
#define CORTEX_BEZIER_H

#include <cstddef>

#include <vector>
#include <ostream>

/**
 * @brief Bezier curve template implementation
 *
 * Computes vertex and index data suitable for GL_LINES rendering.
 *
 * @tparam T 2D control point type (e.g. glm::vec2). Must support scalar
 *           multiplication, addition, and component access via operator[]
 * @tparam n Degree of the curve
 */
template <typename T, std::size_t n>
struct BezierCurve
{
	using ControlPoint = T;

	/** Control points k[0] through k[n] populated by constructor */
	ControlPoint k[n + 1];

	BezierCurve() {}

	/**
	 * @brief Construct Bezier curve from array of n + 1 control points.
	 *
	 * @param control_points Array of n + 1 control points
	 */
	BezierCurve(const ControlPoint control_points[n + 1]);

	/**
	 * @brief Compute curve position at parameter @p t.
	 *
	 * @param t Curve parameter in [0, 1]
	 * @return Point on the curve at @p t
	 */
	T position(double t) const;

	/**
	 * @brief Compute curve normal at parameter @p t.
	 *
	 * The normal is the tangent vector rotated 90 degrees counter-clockwise.
	 * The result is not normalised.
	 *
	 * @param t Curve parameter in [0, 1]
	 * @return Normal vector at @p t
	 */
	T normal(double t) const;

	/**
	 * @brief Tessellate the curve into vertex and index buffers suitable for
	 *        GL_LINES rendering.
	 *
	 * Appends @p t_count uniformly spaced vertices to @p vertices and appends
	 * GL_LINES indices to @p indices.
	 *
	 * @tparam VertexType 2D Vertex type providing .position and .normal
	 *                    assignable from T.
	 * @tparam IndexType Integer type suitable for array indices
	 *
	 * @param t_count Number of sample points. Must be >= 2.
	 * @param vertices Vertex buffer output. New vertices are appended.
	 * @param indices Index buffer output. New indices are appended.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::size_t t_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
};

/**
 * @brief Bezier surface template implementation
 *
 * Computes vertex and index data suitable for GL_TRIANGLES rendering.
 *
 * @tparam T 3D control point type (e.g. glm::vec3). Must support scalar
 *           multiplication, addition, and component access via operator[]
 * @tparam n Degree in the u direction
 * @tparam m Degree in the v direction
 */
template <typename T, std::size_t n, std::size_t m>
struct BezierSurface
{
	using ControlPoint = T;

	/** Control points k[0..n][0..m] populated by constructor */
	ControlPoint k[n + 1][m + 1];

	BezierSurface() {}

	/**
	 * @brief Construct Bezier surface from (n + 1) x (m + 1) grid of control
	 *        points.
	 *
	 * @param control_points 2D array of (n + 1) x (m + 1) control points
	 */
	BezierSurface(const ControlPoint control_points[n + 1][m + 1]);

	/**
	 * @brief Compute surface position at parameters (@p u, @p v).
	 *
	 * @param u Surface parameter in [0, 1] along the n-degree direction
	 * @param v Surface parameter in [0, 1] along the m-degree direction
	 * @return Point on the surface at (@p u, @p v)
	 */
	T position(double u, double v) const;

	/**
	 * @brief Compute surface normal at parameters (@p u, @p v).
	 *
	 * The normal is the cross product of the partial tangents dp/du x dp/dv.
	 * The result is not normalised.
	 *
	 * @param u Surface parameter in [0, 1] along the n-degree direction
	 * @param v Surface parameter in [0, 1] along the m-degree direction
	 * @return Normal vector at (@p u, @p v)
	 */
	T normal(double u, double v) const;

	/**
	 * @brief Tessellate the surface into vertex and index buffers suitable for
	 *        GL_TRIANGLES rendering.
	 *
	 * Appends @p u_count x @p v_count uniformly spaced vertices to @p vertices
	 * and appends GL_TRIANGLES indices to @p indices.
	 *
	 * @tparam VertexType 3D Vertex type providing .position and .normal
	 *                    assignable from T.
	 * @tparam IndexType Integer type suitable for array indices
	 *
	 * @param u_count Number of sample points along u. Must be >= 2.
	 * @param v_count Number of sample points along v. Must be >= 2.
	 * @param vertices Vertex buffer output. New vertices are appended.
	 * @param indices Index buffer output. New indices are appended.
	 */
	template<typename VertexType, typename IndexType = unsigned int>
	void tessellate(std::size_t u_count, std::size_t v_count, std::vector<VertexType>& vertices, std::vector<IndexType>& indices) const;
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
