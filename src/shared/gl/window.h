/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/window.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps a GLFW window and context. Provides
    a simple way of creating and modifying
    OS windows in a platform-agnostic manner.
*/

#pragma once

#include <string>
#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "core.h"

namespace gl
{
  class window
  {
    public:
      struct frame_buffer_size
      { size_t width, height; };

      window(size_t const width, size_t const height)
        : m_win(nullptr, &glfwDestroyWindow)
      { m_win.reset(glfwCreateWindow(width, height, "", nullptr, nullptr)); }
      window(window const&) = delete;
      window(window &&) = default;
      window& operator =(window const&) = delete;
      window& operator =(window &&) = default;

      bool get_should_close() const
      { return glfwWindowShouldClose(m_win.get()); }
      frame_buffer_size get_frame_buffer_size() const
      {
        int width{}, height{};
        glfwGetFramebufferSize(m_win.get(), &width, &height);
        return { static_cast<size_t>(width), static_cast<size_t>(height) };
      }

      void set_should_close(bool const sc)
      { glfwSetWindowShouldClose(m_win.get(), sc); }
      void set_title(std::string const &title)
      { glfwSetWindowTitle(m_win.get(), title.c_str()); }

      void make_current()
      { glfwMakeContextCurrent(m_win.get()); }
      void swap_buffers()
      { glfwSwapBuffers(m_win.get()); }

      void show()
      { glfwShowWindow(m_win.get()); }
      void hide()
      { glfwHideWindow(m_win.get()); }

    private:
      std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> m_win;
  };
}
