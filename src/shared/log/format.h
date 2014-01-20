/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: log/format.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <string>
#include <stdexcept>
#include <ostream>

namespace logging
{
  class variadic_format
  {
    public:
      variadic_format(std::ostream &os)
        : m_out(os)
      { m_out << std::boolalpha; }

      void print(char const * const format)
      {
        /* Check for remaining occurrences of %%. */
        for(char const *tmp{ format }; *tmp; ++tmp)
        {
          if(tmp && *tmp == '%' && *(tmp + 1) == '%')
          { throw std::out_of_range("Not enough format arguments supplied"); }
        }
        m_out << format;
      }

      template <typename T, typename... Args>
      void print(char const *format, T const &value, Args... args)
      {
        for(; *format; ++format)
        {
          if(*format == '%' && *(format + 1) == '%')
          {
            m_out << value;
            print(format + 2, args...);
            return;
          }
          m_out << *format;
        }
        /* Didn't find %%. */
        throw std::out_of_range("Too many format arguments");
      }

    private:
      std::ostream &m_out;
  };
}
