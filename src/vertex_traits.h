/**
 * @file vertex_traits.h
 *
 * Copyright (c) 2026 Leon Lynch
 *
 * This file is licensed under the terms of the MIT license.
 * See LICENSE file.
 */

#ifndef CORTEX_VERTEX_TRAITS_H
#define CORTEX_VERTEX_TRAITS_H

#include <type_traits>

namespace detail {

/**
 * @brief Trait detecting whether @p T provides a @c .normal data member.
 *
 * Intended for use with @c if @c constexpr (C++17) to populate the @c .normal
 * member conditionally if present.
 *
 * @tparam T Template type to inspect.
 *
 * @par Example
 * @code
 * struct vertex_with_normal_t { glm::vec3 position; glm::vec3 normal; };
 * struct vertex_without_normal_t { glm::vec3 position; };
 *
 * static_assert(detail::has_normal<vertex_with_normal_t>::value);
 * static_assert(!detail::has_normal<vertex_without_normal_t>::value);
 *
 * // Conditional assignment:
 * if constexpr (detail::has_normal<T>::value) {
 *     v.normal = { ... };
 * }
 * @endcode
 * @{
 */
template<typename T, typename = void>
struct has_normal : std::false_type {};
template<typename T>
struct has_normal<T, std::void_t<decltype(std::declval<T&>().normal)>> : std::true_type {};
/// @}

} // namespace detail

#endif
