/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: rpg_application.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#include "rpg_application.h"

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

rpg_application::rpg_application()
{
}

rpg_application::~rpg_application()
{
}

void rpg_application::create_scene()
{
  Ogre::Image img;
  img.load("heightmap.jpg", "General");

  int32_t const size{ static_cast<int32_t>(img.getWidth() * 0.5f) };
  //int32_t const size{ 128 };
  float const scale{ static_cast<float>(size) / img.getWidth() };
  std::cout << "size: " << size << std::endl;
  std::cout << "scale: " << scale << std::endl;

  m_volume.reset(new vox::fixed_volume<uint8_t>({ size, size, size }, [&](vox::vec3<size_t> const &vec)
  {
    auto const col(img.getColourAt((vec.x / scale), (vec.z / scale), 0).r / 2.0f);
    return (vec.y <= size * col) ? 255 : 0;
    //Ogre::Vector3 const p{ vec.x, vec.y, vec.z };
    //Ogre::Real const d{ (p - Ogre::Vector3(size / 2.0f, size / 2.0f, size / 2.0f)).length() };
    //if(d <= 30)
    //{ return 255; }
    //else
    //{ return 0; }
  }));

  m_camera->setPosition(Ogre::Vector3(-size, size, size));
  auto const size2(size >> 1);
  m_camera->lookAt(Ogre::Vector3(size2, 0.0f, size2));

  m_ogre_volume = m_scene_mgr->createManualObject("manual");
  m_ogre_volume->setDynamic(true);
  update_surface();

  Ogre::MaterialManager& lMaterialManager = Ogre::MaterialManager::getSingleton();
  Ogre::MaterialPtr lMaterial{ lMaterialManager.create("M_TEX", "General") };
  Ogre::Technique *lFirstTechnique{ lMaterial->getTechnique(0) };
  Ogre::Pass *lFirstPass{ lFirstTechnique->getPass(0) };
  lFirstPass->setLightingEnabled(true);

  // We create the TextureUnit and tell it to use a specific texture (using it's name).
  // The texture must be available in a resourcegroup at render time.
  Ogre::TextureUnitState *lTextureUnit{ lFirstPass->createTextureUnitState() };
  lTextureUnit->setTextureName("grass.jpg", Ogre::TEX_TYPE_2D);
  lTextureUnit->setTextureCoordSet(0);

  //m_ogre_volume->setMaterialName("M_TEX");
  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(m_ogre_volume);

  std::cout << "initializing lighting" << std::endl;
  Ogre::Light * const light{ m_scene_mgr->createLight("MainLight") };
  light->setPosition(size / 2.0f, size, size / 2.0f);
}

void rpg_application::update_surface()
{
  int32_t const size{ static_cast<int32_t>(m_volume->get_region().get_width()) };

  vox::surface_extractor<vox::triangle<vox::vertex_p>,
                         vox::fixed_volume<uint8_t>> extractor
                           { *m_volume, m_volume->get_region(), 128, m_unit_size };
  vox::surface<vox::triangle<vox::vertex_p>> surface{ extractor() };
  std::cout << "triangles: " << surface.get_triangles().size() << std::endl;

  m_ogre_volume->clear();
  m_ogre_volume->begin("splat", Ogre::RenderOperation::OT_TRIANGLE_LIST);
  //m_ogre_volume->begin("triPlanarMaterial1", Ogre::RenderOperation::OT_TRIANGLE_LIST);

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

bool rpg_application::keyPressed(OIS::KeyEvent const &arg)
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

  return application::keyPressed(arg);
}

//void rpg_application::create_scene()
//{
//  Ogre::Image map;
//  map.load("heightmap.png", "General");
//  std::cout << "texture size is " << map.getWidth() << "x" << map.getHeight() << std::endl;
//
//  auto const center((map.getWidth() >> 1) * 2);
//  auto const center2(center << 1);
//  auto reg(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(center2, 256, center2)));
//
//  PolyVox::RawVolume<uint8_t> tempVolume(reg);
//  PolyVox::LargeVolume<uint8_t> volData(reg);
//
//  std::cout << "populating volume from heightmap with scale " << (center2 / map.getWidth()) << std::endl;
//  {
//    auto const start(std::chrono::system_clock::now());
//    createHeightmapInVolume(tempVolume, map, center2 / map.getWidth());
//    auto const end(std::chrono::system_clock::now());
//    std::cout << "took "
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
//              << "ms" << std::endl;
//  }
//
//  m_camera->setPosition(Ogre::Vector3(center, center, center));
//
//  std::cout << "smoothing volume" << std::endl;
//  {
//    auto const start(std::chrono::system_clock::now());
//    PolyVox::LowPassFilter< PolyVox::RawVolume<uint8_t>, PolyVox::LargeVolume<uint8_t>, int16_t > pass2(&tempVolume, PolyVox::Region(volData.getEnclosingRegion()), &volData, PolyVox::Region(volData.getEnclosingRegion()), 7);
//    pass2.executeSAT();
//    auto const end(std::chrono::system_clock::now());
//    std::cout << "took "
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
//              << "ms" << std::endl;
//  }
//
//  PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;
//
//  std::cout << "creating extractor" << std::endl;
//  PolyVox::MarchingCubesSurfaceExtractor< PolyVox::LargeVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
//
//  std::cout << "extracting into mesh" << std::endl;
//  {
//    auto const start(std::chrono::system_clock::now());
//    surfaceExtractor.execute();
//    auto const end(std::chrono::system_clock::now());
//    std::cout << "took "
//              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
//              << "ms" << std::endl;
//  }
//
//  // Set the scene's ambient light
//  m_scene_mgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
//
//  std::cout << "creating mesh with " << mesh.getNoOfVertices() << " verts" << std::endl;
//
//  //Create a ManualObject
//  Ogre::ManualObject* manual = m_scene_mgr->createManualObject("manual");
//
//  //Begin a section of the ManualObject (we're only using one section)
//  manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);
//
//  //Iterate through all the vertices in the mesh produced by the surface extractor
//  //and add them to the ManualObject section.
//  for(std::vector<PolyVox::PositionMaterialNormal>::const_iterator it = mesh.getVertices().begin(); it != mesh.getVertices().end(); ++it)
//  {
//    const PolyVox::Vector3DFloat& vertexPos = it->getPosition();
//    const PolyVox::Vector3DFloat& vertexNorm = it->getNormal();
//    manual->position(vertexPos.getX(),vertexPos.getY(),vertexPos.getZ());
//    manual->normal(vertexNorm.getX(),vertexNorm.getY(),vertexNorm.getZ());
//    if(vertexPos.getY() > 64)
//    { manual->colour(vertexPos.getY() / 128.0f, 0.0f, 0.0f); }
//    else if(vertexPos.getY() > 32)
//    { manual->colour(vertexPos.getY() / 128.0f, vertexPos.getY() / 128.0f, 0.0f); }
//    else
//    { manual->colour(0.0f, 0.0f, vertexPos.getY() / 128.0f); }
//  }
//
//  //Now we iterate through all the indices from the mesh and also add them to the ManualObject section
//  for(auto const it : mesh.getIndices())
//  { manual->index(it); }
//
//  //End the section then add the ManualObject to the scenegraph
//  manual->end();
//  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);
//
//  std::cout << "initializing lighting" << std::endl;
//
//  // Create a Light and set its position
//  Ogre::Light* light = m_scene_mgr->createLight("MainLight");
//  light->setPosition(20.0f, 80.0f, 50.0f);
//}
