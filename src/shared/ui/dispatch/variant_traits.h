/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/dispatch/variant_traits.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Type traits for mapping Awesomium's Javascript
    objects to C++.
*/

#pragma once

#include <Awesomium/JSValue.h>
#include <Awesomium/STLHelpers.h>

#include <string>
#include <stdexcept>

#include "log/logger.h"

namespace ui
{
  namespace dispatch
  {
    /* Fallback types. */
    template <typename T, typename Enable = void>
    struct variant
    {
      static T get(Awesomium::JSValue const &val)
      { throw std::invalid_argument(std::string{"Unknown type request for variant: ("}
                      + typeid(T).name() + ", " + Awesomium::ToString(val.ToString()) + ")"); }
    };

    /* Integral types. */
    template <typename T>
    struct variant<T, typename std::enable_if<(
                  (std::is_integral<typename std::decay<T>::type>::value
                  && !std::is_same<typename std::decay<T>::type, bool>::value
                  && !std::is_same<typename std::decay<T>::type, char>::value)
                  || std::is_enum<typename std::decay<T>::type>::value)>::type>
    {
      static typename std::decay<T>::type get(Awesomium::JSValue const &val)
      { return static_cast<T>(val.ToInteger()); }
    };

    /* Floating point types. */
    template <typename T>
    struct variant<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
    {
      static T get(Awesomium::JSValue const &val)
      { return static_cast<T>(val.ToDouble()); }
    };

    /* Boolean types. */
    template <>
    struct variant<bool, void>
    {
      static bool get(Awesomium::JSValue const &val)
      { return val.ToBoolean(); }
    };

    /* String types. */
    template <typename T>
    struct variant<T, typename std::enable_if<
                    std::is_same<typename std::decay<T>::type,
                    Awesomium::WebString>::value>::type>
    {
      static Awesomium::WebString get(Awesomium::JSValue const &val)
      { return val.ToString(); }
    };
    template <typename T>
    struct variant<T, typename std::enable_if<
                    std::is_same<typename std::decay<T>::type,
                    std::string>::value>::type>
    {
      static std::string get(Awesomium::JSValue const &val)
      { return Awesomium::ToString(val.ToString()); }
    };

    /* Char type. */
    template <typename T>
    struct variant<T, typename std::enable_if<
                    std::is_same<typename std::decay<T>::type,
                    char>::value>::type>
    {
      static char get(Awesomium::JSValue const &val)
      {
        auto const str(variant<std::string>::get(val));
        log_assert(str.size(), "Invalid size for char");
        return str[0];
      }
    };
  }
}
