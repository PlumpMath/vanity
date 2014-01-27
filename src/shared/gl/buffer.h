/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/buffer.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper around OpenGL's buffer objects.
    This can function as a VBO, IBO, etc.
*/

#pragma once

#include "common.h"
#include "log/logger.h"

#include <type_traits>

namespace gl
{
  template <buffer_target T>
  class buffer
  {
    public:
      static buffer_target constexpr const value{ T };
      using target_t = typename std::underlying_type<buffer_target>::type;
      static target_t constexpr const target_value{ static_cast<target_t>(T) };

      enum class usage
      {
        stream_draw = GL_STREAM_DRAW,
        stream_read = GL_STREAM_READ,
        stream_copy = GL_STREAM_COPY,
        static_draw = GL_STATIC_DRAW,
        static_read = GL_STATIC_READ,
        static_copy = GL_STATIC_COPY
      };

      buffer()
      {
        glCheck(glGenBuffers(1, &m_name));
        log_assert(m_name, "Invalid GL name");
      }
      buffer(buffer const&) = delete;
      buffer(buffer &&) = default;
      ~buffer()
      { glCheck(glDeleteBuffers(1, &m_name)); }
      buffer& operator =(buffer const&) = delete;
      buffer& operator =(buffer &&) = default;

      void bind()
      {
        log_assert(!m_bound, "Buffer is already bound");

        glCheck(glBindBuffer(target_value, m_name));
        m_bound = true;
      }
      void unbind()
      {
        log_assert(m_bound, "Buffer is not bound");

        glCheck(glBindBuffer(target_value, 0));
        m_bound = false;
      }

      /* Pointer types. */
      template <typename C>
      void set_data(C const * const data, GLsizeiptr const size, usage const u)
      {
        log_assert(m_bound, "Buffer is not bound");
        glCheck(glBufferData(target_value, size, data, static_cast<GLenum>(u)));
      }
      /* Container types. */
      template <typename C>
      void set_data(C const &data, usage const u)
      {
        log_assert(m_bound, "Buffer is not bound");
        glCheck(glBufferData(target_value,
                             data.size() * sizeof(typename C::value_type),
                             data.data(), static_cast<GLenum>(u)));
      }

      template <typename C>
      void set_sub_data(size_t const offset, C const * const data, GLsizeiptr const size)
      {
        log_assert(m_bound, "Buffer is not bound");
        glCheck(glBufferSubData(target_value, offset, size, data));
      }
      template <typename C>
      void set_sub_data(size_t const offset, C const &data)
      {
        log_assert(m_bound, "Buffer is not bound");
        glCheck(glBufferSubData(target_value, offset,
                                data.size() * sizeof(typename C::value_type),
                                data.data()));
      }

    private:
      name_t m_name{};
      bool m_bound{ false };
  };
  using vbo = buffer<buffer_target::array>;
  using ibo = buffer<buffer_target::element_array>;
}
