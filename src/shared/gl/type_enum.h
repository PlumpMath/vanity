/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/type_enum.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Provides a type trait mapping from C++
    types to OpenGL type enums.
    i.e. float -> GL_FLOAT
*/

#pragma once

#include <type_traits>

#include "core.h"

namespace gl
{
  template <typename T, typename Enable = void>
  struct type_enum;

  /* Integral types. */
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, int8_t>::value>::type>
  { static constexpr const GLenum value{ GL_BYTE }; };
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, uint8_t>::value>::type>
  { static constexpr const GLenum value{ GL_UNSIGNED_BYTE }; };

  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, int16_t>::value>::type>
  { static constexpr const GLenum value{ GL_SHORT }; };
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, uint16_t>::value>::type>
  { static constexpr const GLenum value{ GL_UNSIGNED_SHORT }; };

  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, int32_t>::value>::type>
  { static constexpr const GLenum value{ GL_INT }; };
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, uint32_t>::value>::type>
  { static constexpr const GLenum value{ GL_UNSIGNED_INT }; };

  /* Floating point types. */
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, float>::value>::type>
  { static constexpr const GLenum value{ GL_FLOAT }; };
  template <typename T>
  struct type_enum<T,
    typename std::enable_if<
      std::is_same<typename std::remove_pointer<typename std::decay<T>::type>::type, double>::value>::type>
  { static constexpr const GLenum value{ GL_DOUBLE }; };
}
