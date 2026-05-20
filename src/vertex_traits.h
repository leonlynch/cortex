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
 * @brief Trait detecting whether @p T provides a @c value_type member type.
 *
 * Intended for use with @c static_assert.
 *
 * @tparam T Template type to inspect.
 *
 * @par Example
 * @code
 * // glm::vec3 defines value_type = float
 * static_assert(detail::has_value_type<glm::vec3>::value);
 *
 * // Plain float has no value_type
 * static_assert(!detail::has_value_type<float>::value);
 * @endcode
 * @{
 */
template<typename T, typename = void>
struct has_value_type : std::false_type {};
template<typename T>
struct has_value_type<T, std::void_t<typename T::value_type>> : std::true_type {};
/// @}

/**
 * @brief Trait detecting whether @p T provides a @c .tangent data member.
 *
 * Intended for use with @c if @c constexpr (C++17) to populate the @c .tangent
 * member conditionally if present.
 *
 * @tparam T Template type to inspect.
 *
 * @par Example
 * @code
 * struct vertex_with_tangent_t { glm::vec3 position; glm::vec3 tangent; };
 * struct vertex_without_tangent_t { glm::vec3 position; };
 *
 * static_assert(detail::has_tangent<vertex_with_tangent_t>::value);
 * static_assert(!detail::has_tangent<vertex_without_tangent_t>::value);
 *
 * // Conditional assignment:
 * if constexpr (detail::has_tangent<T>::value) {
 *     v.tangent = { ... };
 * }
 * @endcode
 * @{
 */
template<typename T, typename = void>
struct has_tangent : std::false_type {};
template<typename T>
struct has_tangent<T, std::void_t<decltype(std::declval<T&>().tangent)>> : std::true_type {};
/// @}

/**
 * @brief Trait detecting whether @p T provides a @c .bitangent data member.
 *
 * Intended for use with @c if @c constexpr (C++17) to populate the @c .bitangent
 * member conditionally if present.
 *
 * @tparam T Template type to inspect.
 *
 * @par Example
 * @code
 * struct vertex_with_bitangent_t { glm::vec3 position; glm::vec3 bitangent; };
 * struct vertex_without_bitangent_t { glm::vec3 position; };
 *
 * static_assert(detail::has_bitangent<vertex_with_bitangent_t>::value);
 * static_assert(!detail::has_bitangent<vertex_without_bitangent_t>::value);
 *
 * // Conditional assignment:
 * if constexpr (detail::has_bitangent<T>::value) {
 *     v.bitangent = { ... };
 * }
 * @endcode
 * @{
 */
template<typename T, typename = void>
struct has_bitangent : std::false_type {};
template<typename T>
struct has_bitangent<T, std::void_t<decltype(std::declval<T&>().bitangent)>> : std::true_type {};
/// @}

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

/**
 * @brief Trait detecting whether @p T provides a @c .texcoord data member.
 *
 * Intended for use with @c if @c constexpr (C++17) to populate the @c .texcoord
 * member conditionally if present.
 *
 * @tparam T Template type to inspect.
 *
 * @par Example
 * @code
 * struct vertex_with_texcoord_t { glm::vec3 position; glm::vec2 texcoord; };
 * struct vertex_without_texcoord_t { glm::vec3 position; };
 *
 * static_assert(detail::has_texcoord<vertex_with_texcoord_t>::value);
 * static_assert(!detail::has_texcoord<vertex_without_texcoord_t>::value);
 *
 * // Conditional assignment:
 * if constexpr (detail::has_texcoord<T>::value) {
 *     v.texcoord = { ... };
 * }
 * @endcode
 * @{
 */
template<typename T, typename = void>
struct has_texcoord : std::false_type {};
template<typename T>
struct has_texcoord<T, std::void_t<decltype(std::declval<T&>().texcoord)>> : std::true_type {};
/// @}

} // namespace detail

#endif
