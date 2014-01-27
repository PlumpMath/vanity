/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/texture.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps an OpenGL texture object. This object can
    be 1d, 2d, 3d, array, et cetera.
*/

#pragma once

#include <string>
#include <type_traits>

#include <glm/vec2.hpp>

#include "common.h"
#include "stb/texture.h"
#include "type_enum.h"

namespace gl
{
  template <texture_target T>
  class texture
  {
    public:
      static texture_target constexpr const value{ T };
      using target_t = typename std::underlying_type<texture_target>::type;
      static target_t constexpr const target_value{ static_cast<target_t>(T) };

      texture(std::string const &file)
      {
        glCheck(glGenTextures(1, &m_name));
        log_assert(m_name, "Invalid texture name");
        bind();

        glCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
        glCheck(glTexParameteri(target_value, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        glCheck(glTexParameteri(target_value, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        glCheck(glTexParameteri(target_value, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        glCheck(glTexParameteri(target_value, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        stb::texture const tex(file);
        m_size.x = tex.width;
        m_size.y = tex.height;
        log_debug("Loaded %% with %%x%%:%%", file, m_size.x, m_size.y, tex.depth);

        glCheck(glTexImage2D(
             /* target */ target_value,
             /* mipmap */ 0,
             /* internal */ GL_RGBA8,
             /* size */ m_size.x, m_size.y,
             /* border */ 0,
             /* external */ static_cast<GLenum>(tex.format),
             /* size type */ type_enum<stb::texture::data_t>::value, 
             /* data */ tex.data.get()
            ));

        unbind();
      }
      texture(texture const&) = delete;
      texture(texture &&) = default;
      ~texture()
      { glCheck(glDeleteTextures(1, &m_name)); }
      texture& operator =(texture const&) = delete;
      texture& operator =(texture &&) = default;

      void bind(GLenum const unit = 0)
      {
        log_assert(!m_bound, "Texture is already bound");

        glCheck(glActiveTexture(GL_TEXTURE0 + unit));
        glCheck(glBindTexture(target_value, m_name));
        m_bound = true;
        m_unit = unit;
      }
      void unbind()
      {
        log_assert(m_bound, "Texture is not bound");

        glCheck(glBindTexture(target_value, 0));
        m_bound = false;
        m_unit = 0;
      }

      GLenum get_unit() const
      { return m_unit; }

    private:
      name_t m_name{};
      bool m_bound{ false };

      /* The unit we're bound to. */
      GLenum m_unit{}; 
      glm::ivec2 m_size;
      std::string m_filename;
  };
  using texture_1d = texture<texture_target::_1d>;
  using texture_2d = texture<texture_target::_2d>;
  using texture_3d = texture<texture_target::_3d>;
}
