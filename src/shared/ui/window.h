/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/window.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <cef_browser.h>

#include <OgreEntity.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include "client.h"

namespace ui
{
  class window
  {
    public:
      window(borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
             float const w, float const h);
      ~window();

      void load(borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
                float const w, float const h);

      /* Mouse positions are screen-relative. */
      void key_pressed(OIS::KeyEvent const &arg);
      void key_released(OIS::KeyEvent const &arg);
      void mouse_moved(OIS::MouseEvent const &arg);
      void mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id);
      void mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id);

    private:
      std::string m_url;
      Ogre::TexturePtr m_texture;
      borrowed_ptr<Ogre::Entity> m_entity;
      CefRefPtr<CefBrowser> m_browser;
      CefRefPtr<client> m_client;
  };

  inline bool operator ==(window const &lhs, window const &rhs)
  { return &lhs == &rhs; }
}
