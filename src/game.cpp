/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: game.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "game.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <limits>
#include <cmath>

#include <OgreVector3.h>
#include <OgreImage.h>
#include <OgreMaterialManager.h>

#include "vox/fixed_volume.h"
#include "vox/surface_extractor.h"
#include "vox/triangle.h"
#include "vox/vertex.h"

#include <Awesomium/WebCore.h>
#include <Awesomium/BitmapSurface.h>
#include <Awesomium/STLHelpers.h>

#include "ui/window.h"
#include "ui/input_dispatcher.h"
#include "ui/server.h"

#include "log/logger.h"

game::game()
{
}

game::~game()
{
}

void game::create_scene()
{
  log_debug("Creating scene");
  log_scoped_push();

  Ogre::Image img;
  img.load("heightmap.jpg", "General");
  log_info("heightmap size: %%x%%", img.getWidth(), img.getHeight());

  int32_t const size{ static_cast<int32_t>(img.getWidth() * 0.1f) };
  float const scale{ static_cast<float>(size) / img.getWidth() };
  log_info("size: %%", size);
  log_info("scale: %%", scale);

  log_info("voxelizing...");
  log_push();
  auto const start(std::chrono::system_clock::now());
  m_volume.reset(new vox::fixed_volume<uint8_t>({ size, static_cast<size_t>(256 * 1.5f), size },
  [&](vox::vec3<size_t> const &vec)
  {
    auto const col(img.getColourAt((vec.x / scale), (vec.z / scale), 0).r / 2.0f);
    return (vec.y <= size * col) ? 255 : 0;
  }));
  auto const end(std::chrono::system_clock::now());
  log_pop();
  log_info("voxelized: %%ms",
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

  m_camera->setPosition(Ogre::Vector3(-size, size, size));
  auto const size2(size >> 1);
  m_camera->lookAt(Ogre::Vector3(size2, 0.0f, size2));

  m_ogre_volume = m_scene_mgr->createManualObject("terrain");
  m_ogre_volume->setDynamic(true);
  update_surface();
  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(m_ogre_volume);

  log_info("initializing lighting");
  Ogre::Light * const light{ m_scene_mgr->createLight("MainLight") };
  light->setPosition(size / 2.0f, size, size / 2.0f);

  m_ui_server.reset(new ui::server(m_scene_mgr));
  auto *win(new std::unique_ptr<ui::window>(m_ui_server->create_window("http://duckduckgo.com", 1024, 768)));
  (*win)->focus();
}

void game::update_surface()
{
  int32_t const size{ static_cast<int32_t>(m_volume->get_region().get_width()) };

  vox::surface_extractor<vox::triangle_p,
                         vox::fixed_volume<uint8_t>> extractor
                           { *m_volume, m_volume->get_region(), 128, m_unit_size };
  vox::surface<vox::triangle_p> surface{ extractor() };
  log_debug("triangles: %%", surface.get_triangles().size());

  m_ogre_volume->clear();
  m_ogre_volume->begin("splat", Ogre::RenderOperation::OT_TRIANGLE_LIST);

  auto const &triangles(surface.get_triangles());
  for(size_t i(0); i < triangles.size(); ++i)
  {
    for(size_t k(0); k < 3; ++k)
    {
      m_ogre_volume->position(triangles[i].verts[k].p.x,
                              triangles[i].verts[k].p.y,
                              triangles[i].verts[k].p.z);

      m_ogre_volume->textureCoord(triangles[i].verts[k].p.x * 0.001f,
                                  triangles[i].verts[k].p.z * 0.001f);

      auto const h(std::min(1.0f, triangles[i].verts[k].p.y / (size * 0.3f)));
      auto const h_inv(std::max(0.0f, 0.3f - h));

      if(triangles[i].verts[k].p.y > (size * 0.2f))
      { m_ogre_volume->colour(h, h_inv, 0.0f); }
      else if(triangles[i].verts[k].p.y > (size * 0.1f))
      { m_ogre_volume->colour(h, h, 0.0f); }
      else
      { m_ogre_volume->colour(0.0f, 0.0f, h); }

      m_ogre_volume->normal(triangles[i].normal.x,
                            triangles[i].normal.y,
                            triangles[i].normal.z);
    }
  }

  m_ogre_volume->end();
}

bool game::key_pressed(OIS::KeyEvent const &arg)
{
  if(arg.key == OIS::KC_C)
  { update_surface(); }
  else if(arg.key == OIS::KC_EQUALS)
  {
    ++m_unit_size;
    update_surface();
    log_debug("unit size: %%", m_unit_size);
  }
  else if(arg.key == OIS::KC_MINUS)
  {
    if(m_unit_size > 1)
    { --m_unit_size; }
    update_surface();
    log_debug("unit size: %%", m_unit_size);
  }

  m_camera_mgr->injectKeyDown(arg);

  ui::input_dispatcher::global().key_pressed(arg);

  return true;
}

bool game::key_released(OIS::KeyEvent const &arg)
{
  m_camera_mgr->injectKeyUp(arg);

  ui::input_dispatcher::global().key_released(arg);

  return true;
}

bool game::frame_rendering_queued(Ogre::FrameEvent const &evt)
{
  m_ui_server->update();
  return true;
}

bool game::mouse_moved(OIS::MouseEvent const &arg)
{
  //m_camera_mgr->injectMouseMove(arg);

  ui::input_dispatcher::global().mouse_moved(arg);

  return true;
}

bool game::mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
{
  m_camera_mgr->injectMouseDown(arg, id);

  ui::input_dispatcher::global().mouse_pressed(arg, id);

  return true;
}

bool game::mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
{
  m_camera_mgr->injectMouseUp(arg, id);

  ui::input_dispatcher::global().mouse_released(arg, id);

  return true;
}
