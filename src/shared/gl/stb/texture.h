/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: gl/stb/texture.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper for an STB-loaded OpenGL
    texture image. These are loaded from
    disk and can be of a variety of formats.
*/

#pragma once

#include <string>
#include <memory>

#include "image.h"
#include "log/logger.h"

namespace gl
{
  namespace stb
  {
    struct texture
    {
      using data_t = unsigned char;

      texture() = delete;
      texture(std::string const &file)
        : data(nullptr, &stbi_image_free)
      {
        int x{}, y{}, n{};
        data.reset(stbi_load(file.c_str(), &x, &y, &n, 0));
        log_assert(data, "Unable to load image: %%", file);

        switch(n)
        {
          case 3:
            format = texture_format::rgb;
            break;
          case 4:
            format = texture_format::rgba;
            break;
          default:
            log_fail("Invalid texture depth");
            break;
        }

        width = x;
        height = y;
        depth = n;
      }
      texture(texture const&) = delete;
      texture(texture &&) = default;
      texture& operator =(texture const&) = delete;
      texture& operator =(texture &&) = default;

      std::unique_ptr<data_t, decltype(&stbi_image_free)> data;
      size_t width{}, height{}, depth{};
      texture_format format{ texture_format::rgba };
    };
  }
}
