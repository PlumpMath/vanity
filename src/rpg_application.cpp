#include "rpg_application.h"

#include <iostream>
#include <vector>
#include <chrono>

#include <OgreVector3.h>
#include <OgreImage.h>

#include "PolyVoxCore/CubicSurfaceExtractorWithNormals.h"
#include "PolyVoxCore/MarchingCubesSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/LargeVolume.h"
#include "PolyVoxCore/LowPassFilter.h"
#include "PolyVoxCore/MaterialDensityPair.h"
#include <cmath>

rpg_application::rpg_application()
{
}

rpg_application::~rpg_application()
{
}

template <typename T>
void createHeightmapInVolume(T &volData, Ogre::Image const &map, size_t scale)
{
  auto col(0.0f);
  for(size_t z(1); z < volData.getDepth() - 1; ++z)
  {
    for(size_t x(1); x < volData.getWidth() - 1; ++x)
    {
      col = map.getColourAt((x / scale), (z / scale), 0).r / 2.0f;

      for(size_t y(0); y < volData.getHeight(); ++y)
      {
        if((y <= volData.getDepth() * col))
        { volData.setVoxelAt(x, y, z, 255); }
        else
        { volData.setVoxelAt(x, y, z, 0); }
      }
    }
  }
}

void rpg_application::create_scene()
{
  Ogre::Image map;
  map.load("heightmap.png", "General");
  std::cout << "texture size is " << map.getWidth() << "x" << map.getHeight() << std::endl;

  auto const center((map.getWidth() >> 1) * 2);
  auto const center2(center << 1);
  auto reg(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(center2, 256, center2)));

  PolyVox::RawVolume<uint8_t> tempVolume(reg);
  PolyVox::LargeVolume<uint8_t> volData(reg);

  std::cout << "populating volume from heightmap with scale " << (center2 / map.getWidth()) << std::endl;
  {
    auto const start(std::chrono::system_clock::now());
    createHeightmapInVolume(tempVolume, map, center2 / map.getWidth());
    auto const end(std::chrono::system_clock::now());
    std::cout << "took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms" << std::endl;
  }

  m_camera->setPosition(Ogre::Vector3(center, center, center));

  std::cout << "smoothing volume" << std::endl;
  {
    auto const start(std::chrono::system_clock::now());
    PolyVox::LowPassFilter< PolyVox::RawVolume<uint8_t>, PolyVox::LargeVolume<uint8_t>, int16_t > pass2(&tempVolume, PolyVox::Region(volData.getEnclosingRegion()), &volData, PolyVox::Region(volData.getEnclosingRegion()), 7);
    pass2.executeSAT();
    auto const end(std::chrono::system_clock::now());
    std::cout << "took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms" << std::endl;
  }

  PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;

  std::cout << "creating extractor" << std::endl;
  PolyVox::MarchingCubesSurfaceExtractor< PolyVox::LargeVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);

  std::cout << "extracting into mesh" << std::endl;
  {
    auto const start(std::chrono::system_clock::now());
    surfaceExtractor.execute();
    auto const end(std::chrono::system_clock::now());
    std::cout << "took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms" << std::endl;
  }

  // Set the scene's ambient light
  m_scene_mgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

  std::cout << "creating mesh with " << mesh.getNoOfVertices() << " verts" << std::endl;

  //Create a ManualObject
  Ogre::ManualObject* manual = m_scene_mgr->createManualObject("manual");

  //Begin a section of the ManualObject (we're only using one section)
  manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

  //Iterate through all the vertices in the mesh produced by the surface extractor
  //and add them to the ManualObject section.
  for(std::vector<PolyVox::PositionMaterialNormal>::const_iterator it = mesh.getVertices().begin(); it != mesh.getVertices().end(); ++it)
  {
    const PolyVox::Vector3DFloat& vertexPos = it->getPosition();
    const PolyVox::Vector3DFloat& vertexNorm = it->getNormal();
    manual->position(vertexPos.getX(),vertexPos.getY(),vertexPos.getZ());
    manual->normal(vertexNorm.getX(),vertexNorm.getY(),vertexNorm.getZ());
    if(vertexPos.getY() > 64)
    { manual->colour(vertexPos.getY() / 128.0f, 0.0f, 0.0f); }
    else if(vertexPos.getY() > 32)
    { manual->colour(vertexPos.getY() / 128.0f, vertexPos.getY() / 128.0f, 0.0f); }
    else
    { manual->colour(0.0f, 0.0f, vertexPos.getY() / 128.0f); }
  }

  //Now we iterate through all the indices from the mesh and also add them to the ManualObject section
  for(auto const it : mesh.getIndices())
  { manual->index(it); }

  //End the section then add the ManualObject to the scenegraph
  manual->end();
  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

  std::cout << "initializing lighting" << std::endl;

  // Create a Light and set its position
  Ogre::Light* light = m_scene_mgr->createLight("MainLight");
  light->setPosition(20.0f, 80.0f, 50.0f);
}
