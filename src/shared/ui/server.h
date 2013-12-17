/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/server.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <memory>

#include <Awesomium/WebCore.h>

#include "surface_factory.h"
#include "window.h"

namespace ui
{
  class server
  {
    public:
      server(borrowed_ptr<Ogre::SceneManager> const mgr)
        : m_web_core(Awesomium::WebCore::Initialize(Awesomium::WebConfig()))
        , m_surface_factory(new ui::surface_factory)
        , m_scene_manager(mgr)
      { m_web_core->set_surface_factory(m_surface_factory.get()); }

      ~server()
      { Awesomium::WebCore::Shutdown(); }

      server(server const &) = delete;
      server& operator =(server const &) = delete;

      std::unique_ptr<window> create_window(std::string const &url,
                                            size_t const width, size_t const height)
      {
        std::unique_ptr<window> win(new window(this, m_scene_manager, url, width, height));
        return win;
      }

      void update() const
      { m_web_core->Update(); }

      borrowed_ptr<Awesomium::WebCore> get_web_core() const
      { return m_web_core; }

    private:
      borrowed_ptr<Awesomium::WebCore> const m_web_core;
      std::unique_ptr<surface_factory> const m_surface_factory;
      borrowed_ptr<Ogre::SceneManager> const m_scene_manager; 
  };
}
