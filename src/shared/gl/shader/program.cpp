/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/shader/program.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "program.h"

namespace gl
{
  namespace shader
  {
    template <typename T>
    void check_linkage(T const n)
    {
      std::array<GLchar, 512> buf;
      GLint res{};
      glCheck(glGetProgramiv(n, GL_LINK_STATUS, &res));
      if(!res)
      {
        glCheck(glGetShaderInfoLog(n, buf.size(), nullptr, buf.data()));
        log_error("Failed to link program:");
        log_scoped_push();
        log_error("%%", buf.data());

        log_fail("Exiting");
      }
    }

    program_impl<build::debug>::program_impl(vertex_step::source_t const &vs,
                                             geometry_step::source_t const &gs,
                                             fragment_step::source_t const &fs)
      : m_name(new name_t{}, &destroy_name)
    {
      glCheck(*m_name = glCreateProgram());

      m_vert_step.compile(vs);
      m_geo_step.compile(gs);
      m_frag_step.compile(fs);

      log_assert(m_vert_step.get_is_compiled() && m_frag_step.get_is_compiled(),
          "Shader program did not compile sufficiently");

      m_vert_step.attach(*m_name);
      if(m_geo_step.get_is_compiled())
      { m_geo_step.attach(*m_name); }
      m_frag_step.attach(*m_name);

      glCheck(glLinkProgram(*m_name));
      check_linkage(*m_name);
    }

    void program_impl<build::debug>::bind()
    {
      log_assert(!m_bound, "Shader program is already bound");
      glCheck(glUseProgram(*m_name));
      m_bound = true;
    }

    void program_impl<build::debug>::unbind()
    {
      log_assert(m_bound, "Shader program is not bound");
      glCheck(glUseProgram(0));
      m_bound = false;
    }

    GLint program_impl<build::debug>::get_uniform_location(std::string const &u)
    {
      log_assert(m_bound, "Shader program is not bound");

      glCheck(auto const ul(glGetUniformLocation(*m_name, u.c_str())));
      if(ul < 0)
      { log_error("Failed to find uniform '%%'", u); }
      return ul;
    }

    void program_impl<build::debug>::update_uniform(GLint const loc, GLint const i)
    {
      log_assert(m_bound, "Shader program is not bound");
      glCheck(glUniform1i(loc, i));
    }

    void program_impl<build::debug>::update_uniform(GLint const loc, GLfloat const f)
    {
      log_assert(m_bound, "Shader program is not bound");
      glCheck(glUniform1f(loc, f));
    }
  }
}
