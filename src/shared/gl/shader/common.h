/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/shader/common.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Commonly used headers and types for
    the OpenGL shader wrappers.
*/

#pragma once

#include <string>
#include <memory>

#include "core.h"
#include "log/logger.h"

namespace gl
{
  namespace shader
  {
    enum class build
    { debug, release };
  }
}
