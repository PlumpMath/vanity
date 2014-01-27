/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/check.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Provides a macro for checking GL errors.
    This should be used on *every single* GL
    call. Literally, every single one. Period.
*/

#pragma once

#include <vector>

#include "core.h"
#include "log/logger.h"

#if VANITY_DEBUG
#define glCheck(func) \
    func; \
    gl::check_internal(#func, __FILE__, __LINE__)
#else
#define glCheck(func) func
#endif

namespace gl
{
  static inline char const * const error_internal(GLenum const err) 
  {
    switch(err)
    {
      case GL_INVALID_ENUM:
        return "Invalid enum";
      case GL_INVALID_VALUE:
        return "Invalid value";
      case GL_INVALID_OPERATION:
        return "Invalid operation";
      case GL_STACK_OVERFLOW:
        return "Stack overflow";
      case GL_STACK_UNDERFLOW:
        return "Stack underflow";
      case GL_OUT_OF_MEMORY:
        return "Out of memory";
      default:
        return "Unknown error";
    }
  }

  static inline void check_internal(char const * const func, char const * const file, size_t const line)
  {
    std::vector<GLenum> errors;
    while(true)
    {
      GLenum const err{ glGetError() };
      if(err == GL_NO_ERROR)
      { break; }
      errors.push_back(err);
    }

    for(auto it(errors.cbegin()); it != errors.cend(); ++it)
    { log_error("OpenGL error %% (%%) at %%:%% - %%", *it, error_internal(*it), file, line, func); }

    log_assert(errors.size() == 0, "OpenGL error(s)!");
  }
}
