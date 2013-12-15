/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/grid_cell.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include "vec3.h"

namespace vox
{
  template <typename Value>
  struct grid_cell
  {
    vec3<float> p[8];
    vec3<float> n[8];
    Value val[8]{};
  };
}
