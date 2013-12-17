/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/surface.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    A surface compatible with Awesomium.
    It updates an Ogre texture each time the web view
    changes contents. The implementation is some bullshit
    and was copied, almost verbatim, from sample code.
*/

#pragma once

#include <Awesomium/WebCore.h>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreFrameListener.h>
#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>

namespace ui
{
  class surface : public Awesomium::Surface
  {
    public:
      surface(size_t const width, size_t const height);

      void Paint(unsigned char *src_buffer, int src_row_span,
          Awesomium::Rect const &src_rect, Awesomium::Rect const &dest_rect) override;
      void Scroll(int dx, int dy, Awesomium::Rect const &clip_rect) override;

    private:
      Ogre::TexturePtr m_texture;
      static int32_t constexpr bytes_per_pixel{ 4 }; /* BGRA */
  };
}
