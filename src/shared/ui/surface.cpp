/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/surface.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "surface.h"

namespace ui
{
  surface::surface(size_t const width, size_t const height)
  {
    m_texture = Ogre::TextureManager::getSingleton().createManual(
        "ui_" + std::to_string(42),
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D, width, height, 0, Ogre::PF_A8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY);
  }

  void surface::Paint(unsigned char *src_buffer, int src_row_span,
      Awesomium::Rect const &src_rect, Awesomium::Rect const &dest_rect) 
  {
    Ogre::HardwarePixelBufferSharedPtr tex_buf{ m_texture->getBuffer() };
    tex_buf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    {
      auto * const data(static_cast<decltype(src_buffer)>(tex_buf->getCurrentLock().data));
      auto const rowspan(m_texture->getWidth() * bytes_per_pixel);
      for(int row{}; row < dest_rect.height; ++row)
      {
        std::memcpy(data + (row + dest_rect.y) * rowspan + (dest_rect.x * bytes_per_pixel),
            src_buffer + (row + src_rect.y) * src_row_span + (src_rect.x * bytes_per_pixel),
            dest_rect.width * bytes_per_pixel);
      }
    } tex_buf->unlock();
  }

  void surface::Scroll(int dx, int dy, Awesomium::Rect const &clip_rect) 
  {
    if(std::abs(dx) >= clip_rect.width || std::abs(dy) >= clip_rect.height)
    { return; }

    Ogre::HardwarePixelBufferSharedPtr tex_buf{ m_texture->getBuffer() };
    tex_buf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
    {
      auto * const data(static_cast<unsigned char*>(tex_buf->getCurrentLock().data));
      auto const rowspan(m_texture->getWidth() * bytes_per_pixel);
      if(dx < 0 && dy == 0)
      {
        /* Area shifted left by dx. */
        std::vector<unsigned char> tmp((clip_rect.width + dx) * bytes_per_pixel);

        for(int i{}; i < clip_rect.height; ++i)
        {
          std::memcpy(tmp.data(), data + (i + clip_rect.y) * rowspan +
              (clip_rect.x - dx) * bytes_per_pixel, (clip_rect.width + dx) * bytes_per_pixel);
          std::memcpy(data + (i + clip_rect.y) * rowspan + (clip_rect.x) * bytes_per_pixel,
              tmp.data(), (clip_rect.width + dx) * bytes_per_pixel);
        }
      }
      else if(dx > 0 && dy == 0)
      {
        /* Area shifted right by dx. */
        std::vector<unsigned char> tmp((clip_rect.width - dx) * bytes_per_pixel);

        for(int i{}; i < clip_rect.height; ++i)
        {
          std::memcpy(tmp.data(), data + (i + clip_rect.y) * rowspan +
              (clip_rect.x) * bytes_per_pixel, (clip_rect.width - dx) * bytes_per_pixel);
          std::memcpy(data + (i + clip_rect.y) * rowspan + (clip_rect.x + dx) * bytes_per_pixel,
              tmp.data(), (clip_rect.width - dx) * bytes_per_pixel);
        }
      }
      else if(dy < 0 && dx == 0)
      {
        /* Area shifted down by dy. */
        for(int i{}; i < clip_rect.height + dy; ++i)
        {
          std::memcpy(data + (i + clip_rect.y) * rowspan + (clip_rect.x * bytes_per_pixel),
              data + (i + clip_rect.y - dy) * rowspan + (clip_rect.x * bytes_per_pixel),
              clip_rect.width * bytes_per_pixel);
        }
      }
      else if(dy > 0 && dx == 0)
      {
        /* Area shifted up by dy. */
        for(int i{ clip_rect.height - 1 }; i >= dy; --i)
        {
          std::memcpy(data + (i + clip_rect.y) * rowspan + (clip_rect.x * bytes_per_pixel),
              data + (i + clip_rect.y - dy) * rowspan + (clip_rect.x * bytes_per_pixel),
              clip_rect.width * bytes_per_pixel);
        }
      }
    } tex_buf->unlock();
  }
}
