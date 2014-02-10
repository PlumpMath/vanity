/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/shader/step.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "step.h"
#include <array>

namespace gl
{
  namespace shader
  {
    void destroy_step_name(GLuint const * const n)
    { glCheck(glDeleteShader(*n)); }

    template <typename T>
    void check_compilation(T const n)
    {
      std::array<GLchar, 512> buf;
      GLint res{};
      glCheck(glGetShaderiv(n, GL_COMPILE_STATUS, &res));
      if(!res)
      {
        glCheck(glGetShaderInfoLog(n, buf.size(), nullptr, buf.data()));
        log_error("Failed to compile shader:");
        log_scoped_push();
        log_error("%%", buf.data());

        log_fail("Exiting");
      }
    }

    inline std::string read_file(std::string const &filename)
    { 
      std::ifstream is(filename); 
      log_assert(is.is_open(), "Failed to open shader file: %%", filename);

      return std::string(std::istreambuf_iterator<char>(is),
                         std::istreambuf_iterator<char>());
    }

    template <typename Name, typename Source>
    bool compile_impl(Name const &name, Source const &source)
    {
      if(source.empty())
      { return false; }

      std::string const contents(read_file(source));
      if(contents.empty())
      {
        log_error("Shader file empty: %%", source);
        return false;
      }

      auto const * tmp(contents.c_str());
      glCheck(glShaderSource(name, 1, &tmp, nullptr));
      glCheck(glCompileShader(name));

      check_compilation(name);
      return true;
    }

    void vertex_step_impl<build::debug>::compile(vertex_step_impl<build::debug>::source_t const &s)
    {
      if(!compile_impl(*m_name, s))
      { return; }

      m_source = s;
      m_compiled = true;
    }

    void vertex_step_impl<build::debug>::attach(vertex_step_impl<build::debug>::name_t const n)
    {
      log_assert(m_compiled, "Cannot attach uncompiled shader");
      glCheck(glAttachShader(n, *m_name));
    }

    void geometry_step_impl<build::debug>::compile(geometry_step_impl<build::debug>::source_t const &s)
    {
      if(!compile_impl(*m_name, s))
      { return; }

      m_source = s;
      m_compiled = true;
    }

    void geometry_step_impl<build::debug>::attach(geometry_step_impl<build::debug>::name_t const n)
    {
      log_assert(m_compiled, "Cannot attach uncompiled shader");
      glCheck(glAttachShader(n, *m_name));
    }

    void fragment_step_impl<build::debug>::compile(fragment_step_impl<build::debug>::source_t const &s)
    {
      if(!compile_impl(*m_name, s))
      { return; }

      m_source = s;
      m_compiled = true;
    }

    void fragment_step_impl<build::debug>::attach(fragment_step_impl<build::debug>::name_t const n)
    {
      log_assert(m_compiled, "Cannot attach uncompiled shader");
      glCheck(glAttachShader(n, *m_name));
    }
  }
}
