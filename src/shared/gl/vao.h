/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/vao.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps an OpenGL vertex array object.
    Provides facilities for mapping vertex
    attributes and maintains state and error
    checking.
*/

#pragma once

#include "common.h"
#include "type_enum.h"

namespace gl
{
  enum class normalized : GLenum
  { yes, no };

  enum class attribute : GLenum
  { position, color, normal, tex_coord };

  class vao
  {
    public:
      vao()
      {
        glCheck(glGenVertexArrays(1, &m_name));
        log_assert(m_name, "Invalid GL name");
      }
      vao(vao const&) = delete;
      vao(vao &&) = default;
      ~vao()
      { glCheck(glDeleteVertexArrays(1, &m_name)); }
      vao& operator =(vao const&) = delete;
      vao& operator =(vao &&) = default;

      void bind()
      {
        log_assert(!m_bound, "VAO is already bound");
        glCheck(glBindVertexArray(m_name));
        m_bound = true;
      }
      void unbind()
      {
        log_assert(m_bound, "VAO is not bound");
        glCheck(glBindVertexArray(0));
        m_bound = false;
      }

      void enable_vertex_attrib_array(attribute const a)
      {
        log_assert(m_bound, "VAO is not bound");
        glCheck(glEnableVertexAttribArray(static_cast<GLenum>(a)));
      }

      template <typename D>
      void vertex_attrib_pointer(attribute const a, GLint const size, GLsizei const stride,
                                 normalized const n = normalized::no, void const * const ptr = nullptr)
      {
        log_assert(m_bound, "VAO is not bound");

        glCheck(glVertexAttribPointer(static_cast<GLenum>(a), size, type_enum<D>::value,
                                      static_cast<GLenum>(n), stride, ptr));
      }

      template <typename D>
      void vertex_attrib_i_pointer(attribute const a, GLint const size, 
                                   GLsizei const stride, void const * const ptr = nullptr)
      {
        log_assert(m_bound, "VAO is not bound");

        glCheck(glVertexAttribIPointer(static_cast<GLenum>(a), size, type_enum<D>::value,
                                       stride, ptr));
      }


    private:
      name_t m_name{};
      bool m_bound{ false };
  };
}
