/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/shader/program.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps an OpenGL (GLSL) shader program, which
    is a compiled and linked binary of a vertex,
    fragment, and (optionally) geometry shaders.
*/

#pragma once

#include "common.h"
#include "step.h"
#include "texture.h"

#include <array>

namespace gl
{
  namespace shader
  {
    template <build B>
    class program_impl;

    template <>
    class program_impl<build::debug>
    {
      public:
        using name_t = GLuint;

        program_impl() = delete;
        program_impl(vertex_step::source_t const &vs,
                     geometry_step::source_t const &gs,
                     fragment_step::source_t const &fs);
        program_impl(program_impl const&) = delete;
        program_impl(program_impl &&) = default;
        program_impl& operator =(program_impl const&) = delete;
        program_impl& operator =(program_impl &&) = default;


        void bind();
        void unbind();

        GLint get_uniform_location(std::string const &u);

        void update_uniform(GLint const loc, GLint const i);
        void update_uniform(GLint const loc, GLfloat const f);
        template <texture_target T>
        void update_uniform(GLint const loc, texture<T> const &t)
        {
          log_assert(m_bound, "Shader program is not bound");
          glCheck(glUniform1i(loc, t.get_unit()));
        }

      private:
        static void destroy_name(name_t const * const n)
        { glCheck(if(glIsProgram(*n)) { glDeleteProgram(*n); }); }

        std::unique_ptr<name_t, decltype(&destroy_name)> m_name;
        vertex_step m_vert_step;
        geometry_step m_geo_step;
        fragment_step m_frag_step;

        bool m_bound{ false };
    };

#if VANITY_DEBUG
    using program = program_impl<build::debug>;
#else
    using program = program_impl<build::release>;
#endif
  }
}
