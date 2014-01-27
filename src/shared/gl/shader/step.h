/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/shader/step.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps an OpenGL (GLSL) shader program step,
    which can be either vertex, geometry, or
    fragment.
*/

#pragma once

#include "../common.h"
#include "common.h"

#include <fstream>

namespace gl
{
  namespace shader
  {
    /* Detaches and deletes a step name. */
    void destroy_step_name(GLuint const * const);

    template <build B>
    class vertex_step_impl;

    template <>
    class vertex_step_impl<build::debug>
    {
      public:
        using name_t = GLuint;
        using source_t = std::string;

        vertex_step_impl()
          : m_name(new name_t{}, &destroy_step_name)
        { glCheck(*m_name = glCreateShader(GL_VERTEX_SHADER)); }
        vertex_step_impl(vertex_step_impl const&) = delete;
        vertex_step_impl(vertex_step_impl &&) = default;
        vertex_step_impl& operator =(vertex_step_impl const&) = delete;
        vertex_step_impl& operator =(vertex_step_impl &&) = default;

        void compile(source_t const &s);
        void attach(name_t const n);

        bool get_is_compiled() const
        { return m_compiled; }

      private:
        std::unique_ptr<name_t, decltype(&destroy_step_name)> m_name;
        source_t m_source;
        bool m_compiled{ false };
    };

    template <build B>
    class geometry_step_impl;

    template <>
    class geometry_step_impl<build::debug>
    {
      public:
        using name_t = GLuint;
        using source_t = std::string;

        geometry_step_impl()
          : m_name(new name_t{}, &destroy_step_name)
        { glCheck(*m_name = glCreateShader(GL_GEOMETRY_SHADER)); }
        geometry_step_impl(geometry_step_impl const&) = delete;
        geometry_step_impl(geometry_step_impl &&) = default;
        geometry_step_impl& operator =(geometry_step_impl const&) = delete;
        geometry_step_impl& operator =(geometry_step_impl &&) = default;

        void compile(source_t const &s);
        void attach(name_t const n);

        bool get_is_compiled() const
        { return m_compiled; }

      private:
        std::unique_ptr<name_t, decltype(&destroy_step_name)> m_name;
        source_t m_source;
        bool m_compiled{ false };
    };

    template <build B>
    class fragment_step_impl;

    template <>
    class fragment_step_impl<build::debug>
    {
      public:
        using name_t = GLuint;
        using source_t = std::string;

        fragment_step_impl()
          : m_name(new name_t{}, &destroy_step_name)
        { glCheck(*m_name = glCreateShader(GL_FRAGMENT_SHADER)); }
        fragment_step_impl(fragment_step_impl const&) = delete;
        fragment_step_impl(fragment_step_impl &&) = default;
        fragment_step_impl& operator =(fragment_step_impl const&) = delete;
        fragment_step_impl& operator =(fragment_step_impl &&) = default;

        void compile(source_t const &s);
        void attach(name_t const n);

        bool get_is_compiled() const
        { return m_compiled; }

      private:
        std::unique_ptr<name_t, decltype(&destroy_step_name)> m_name;
        source_t m_source;
        bool m_compiled{ false };
    };

#if VANITY_DEBUG
    using vertex_step = vertex_step_impl<build::debug>;
    using geometry_step = geometry_step_impl<build::debug>;
    using fragment_step = fragment_step_impl<build::debug>;
#else
    using vertex_step = vertex_step_impl<build::release>;
    using geometry_step = geometry_step_impl<build::release>;
    using fragment_step = fragment_step_impl<build::release>;
#endif
  }
}
