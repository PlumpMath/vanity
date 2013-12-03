/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: game.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
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

game::game()
{
}

game::~game()
{
}

void game::create_scene()
{
  Ogre::Image img;
  img.load("heightmap.jpg", "General");

  int32_t const size{ static_cast<int32_t>(img.getWidth() * 0.5f) };
  float const scale{ static_cast<float>(size) / img.getWidth() };
  std::cout << "size: " << size << std::endl;
  std::cout << "scale: " << scale << std::endl;

  std::cout << "\nvoxelizing..." << std::endl;
  auto const start(std::chrono::system_clock::now());
  m_volume.reset(new vox::fixed_volume<uint8_t>({ size, 256 * 1.5f, size }, [&](vox::vec3<size_t> const &vec)
  {
    auto const col(img.getColourAt((vec.x / scale), (vec.z / scale), 0).r / 2.0f);
    return (vec.y <= size * col) ? 255 : 0;
  }));
  auto const end(std::chrono::system_clock::now());
  std::cout << "voxelized: "
    << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << "ms" << std::endl << std::endl;

  m_camera->setPosition(Ogre::Vector3(-size, size, size));
  auto const size2(size >> 1);
  m_camera->lookAt(Ogre::Vector3(size2, 0.0f, size2));

  m_ogre_volume = m_scene_mgr->createManualObject("manual");
  m_ogre_volume->setDynamic(true);
  update_surface();
  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(m_ogre_volume);

  std::cout << "initializing lighting" << std::endl;
  Ogre::Light * const light{ m_scene_mgr->createLight("MainLight") };
  light->setPosition(size / 2.0f, size, size / 2.0f);
}

void game::update_surface()
{
  int32_t const size{ static_cast<int32_t>(m_volume->get_region().get_width()) };

  vox::surface_extractor<vox::triangle_p,
                         vox::fixed_volume<uint8_t>> extractor
                           { *m_volume, m_volume->get_region(), 128, m_unit_size };
  vox::surface<vox::triangle_p> surface{ extractor() };
  std::cout << "triangles: " << surface.get_triangles().size() << std::endl;

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
    std::cout << "unit size: " << m_unit_size << std::endl;
  }
  else if(arg.key == OIS::KC_MINUS)
  {
    if(m_unit_size > 1)
    { --m_unit_size; }
    update_surface();
    std::cout << "unit size: " << m_unit_size << std::endl;
  }

  m_camera_mgr->injectKeyDown(arg);

  return true;
}

bool game::key_released(OIS::KeyEvent const &arg)
{
  m_camera_mgr->injectKeyUp(arg);

  return true;
}

bool game::frame_rendering_queued(Ogre::FrameEvent const &evt)
{
  return true;
}

bool game::mouse_moved(OIS::MouseEvent const &arg)
{
  m_camera_mgr->injectMouseMove(arg);

  return true;
}

bool game::mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
{
  m_camera_mgr->injectMouseDown(arg, id);

  return true;
}

bool game::mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
{
  m_camera_mgr->injectMouseUp(arg, id);

  return true;
}
