/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/device.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps the OpenGL context that creates, fuels,
    and maintains GLFW, GLEW, as well as some other
    GL state, such as clear color.
*/

#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "core.h"
#include "../log/logger.h"
#include "check.h"

namespace gl
{
  enum class bit : GLenum
  {
    color_buffer = GL_COLOR_BUFFER_BIT,
    depth_buffer = GL_DEPTH_BUFFER_BIT,
  };
  bit operator |(bit const b1, bit const b2)
  { return static_cast<bit>(static_cast<GLenum>(b1) | static_cast<GLenum>(b2)); }

  class device
  {
    public:
      device()
      {
        glfwSetErrorCallback(&error_callback);

        log_assert(glfwInit(), "Unable to initialize GLFW");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#ifdef VANITY_DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
      }
      device(device const&) = delete;
      device(device &&) = default;
      ~device()
      { glfwTerminate(); }
      device& operator =(device const&) = delete;
      device& operator =(device &&) = default;

      void process_events() /* async */
      { glfwPollEvents(); }
      void wait_for_events() /* sync */
      { glfwWaitEvents(); }

      void clear(bit const bs)
      { glCheck(glClear(static_cast<GLenum>(bs))); }
      void clear_color(float const r, float const g, float const b, float const a)
      { glCheck(glClearColor(r, g, b, a)); }
      void clear_color(int const r, int const g, int const b, int const a)
      { glCheck(glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f)); }

    private:
      static void error_callback(int error, char const * description)
      { log_error("GLFW error %%: %%", error, description); }
  };
}
