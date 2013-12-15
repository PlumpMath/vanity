/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/listener.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreFrameListener.h>
#include <OgreTexture.h>
#include <OgreHardwarePixelBuffer.h>

namespace ui
{
  class listener : public Ogre::FrameListener, public CefRenderHandler
  {
    public:
      listener(Ogre::TexturePtr const texture)
        : m_texture(texture)
      { }

      bool frameStarted(Ogre::FrameEvent const &evt) override
      {
        if(Ogre::Root::getSingletonPtr()->getAutoCreatedWindow()->isClosed())
        { return false; }

        /* TODO: This should only happen once, not per-window. */
        CefDoMessageLoopWork();

        return true;
      }

      bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override
      {
        rect = CefRect(0, 0, m_texture->getWidth(), m_texture->getHeight());
        return true;
      }

      void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
          RectList const &dirtyRects, void const *buffer,
          int width, int height) override
      {
        Ogre::HardwarePixelBufferSharedPtr texBuf{ m_texture->getBuffer() };
        texBuf->lock(Ogre::HardwareBuffer::HBL_DISCARD);
        {
          memcpy(texBuf->getCurrentLock().data, buffer, width * height * 4);
        } texBuf->unlock();
      }

    private:
      Ogre::TexturePtr m_texture;

    public:
      IMPLEMENT_REFCOUNTING(listener)
  };
}
