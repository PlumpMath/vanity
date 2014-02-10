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
#include <thread>

#include "core.h"
#include "log/logger.h"

#if VANITY_DEBUG
#define glCheck(func) \
  if(!gl::check_thread()) \
  { log_fail("Not on GL thread: " #func); } \
  func; \
  gl::check_internal(#func, __FILE__, __LINE__)
#else
#define glCheck(func) func
#endif

namespace gl
{
  /* Allows verification that GL calls are made on the GL thread. */
  void register_thread();
  bool check_thread();

  char const* error_internal(GLenum const err);
  void check_internal(char const * const func, char const * const file, size_t const line);
}
