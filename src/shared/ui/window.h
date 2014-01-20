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

#include <OgreEntity.h>
#include <OgreTexture.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <Awesomium/WebCore.h>

#include "util/borrowed_ptr.h"
#include "dispatch/reciever.h"

namespace ui
{
  class server;
  class surface;

  class window
  {
    public:
      window(borrowed_ptr<server> serv, borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
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

      void focus();
      void unfocus();

    private:
      void init_js();

      std::string m_url;
      Ogre::TexturePtr m_texture;
      borrowed_ptr<Ogre::Entity> m_entity;

      borrowed_ptr<server> m_server;
      std::unique_ptr<Awesomium::WebView,
                      decltype(std::mem_fn(&Awesomium::WebView::Destroy))> m_web_view;
      borrowed_ptr<surface> m_surface;
      dispatch::reciever m_reciever;

      /* The global 'vanity' JS object on which we add functions
       * that affect the whole engine. */
      Awesomium::JSValue m_global_obj;
      /* A JS object specifically for interfacing with this window. */
      Awesomium::JSValue m_win_obj;
  };

  inline bool operator ==(window const &lhs, window const &rhs)
  { return &lhs == &rhs; }
}
