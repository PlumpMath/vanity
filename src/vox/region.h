/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/region.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <cstdint>
#include <cstdlib>

#include "vec3.h"

namespace vox
{
  struct region
  {
      using value_t = int32_t;

      region(vec3<value_t> const &lower, vec3<value_t> const &upper)
        : lower_corner(lower)
        , upper_corner(upper)
      { }
      region(value_t const width, value_t const height, value_t const depth)
        : lower_corner({ 0, 0, 0 })
        , upper_corner({ width, height, depth })
      { }

      value_t get_width() const
      { return std::abs(lower_corner.x - upper_corner.x); }
      value_t get_height() const
      { return std::abs(lower_corner.y - upper_corner.y); }
      value_t get_depth() const
      { return std::abs(lower_corner.z - upper_corner.z); }

      bool contains(region const &reg) const
      {
        bool const lower{ reg.lower_corner.x >= lower_corner.x &&
                          reg.lower_corner.y >= lower_corner.y &&
                          reg.lower_corner.z >= lower_corner.z };
        bool const upper{ reg.upper_corner.x <= upper_corner.x &&
                          reg.upper_corner.y <= upper_corner.y &&
                          reg.upper_corner.z <= upper_corner.z };
        return lower && upper;
      }

      vec3<value_t> const lower_corner, upper_corner;
  };
}
