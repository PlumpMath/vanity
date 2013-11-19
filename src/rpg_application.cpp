#include "rpg_application.h"

#include <iostream>
#include <vector>

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

void createSphereInVolume(PolyVox::LargeVolume<uint8_t>& volData, float fRadius)
{
  //This vector hold the position of the center of the volume
  Ogre::Vector3 v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

  //This three-level for loop iterates over every voxel in the volume
  for (int z = 0; z < volData.getDepth(); z++)
  {
    for (int y = 0; y < volData.getHeight(); y++)
    {
      for (int x = 0; x < volData.getWidth(); x++)
      {
        //Store our current position as a vector...
        Ogre::Vector3 v3dCurrentPos(x,y,z);
        //And compute how far the current position is from the center of the volume
        float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

        uint8_t uVoxelValue = 0;

        //If the current voxel is less than 'radius' units from the center then we make it solid.
        if(fDistToCenter <= fRadius)
        {
          //Our new voxel value
          uVoxelValue = 255;
        }

        //Wrte the voxel value into the volume
        volData.setVoxelAt(x, y, z, uVoxelValue);
      }
    }
  }
}

void createFloorInVolume(PolyVox::LargeVolume<uint8_t>& volData, uint32_t floorHeight)
{
  //This vector hold the position of the center of the volume
  PolyVox::Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

  //This three-level for loop iterates over every voxel in the volume
  for (int z = 1; z < volData.getDepth()-1; z++)
  {
    for (int y = 1; y < volData.getHeight()-1; y++)
    {
      for (int x = 1; x < volData.getWidth()-1; x++)
      {
        uint8_t uVoxelValue = 0;

        //If the current voxel is lower than the floor then we make it solid.
        if(y < floorHeight)
        {
          //Our new voxel value
          uVoxelValue = 255;
        }

        //Wrte the voxel value into the volume   
        volData.setVoxelAt(x, y, z, uVoxelValue);
      }
    }
  }
}

void createHeightmapInVolume(PolyVox::LargeVolume<uint8_t>& volData, Ogre::Image const &map, size_t scale)
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
  std::cout << "woohoo texture size is " << map.getWidth() << "x" << map.getHeight() << std::endl;

  auto const center(map.getWidth());
  auto const center2(center << 1);
  auto reg(PolyVox::Region(PolyVox::Vector3DInt32(0, 0, 0), PolyVox::Vector3DInt32(center2, 256, center2)));
  PolyVox::LargeVolume<uint8_t> volData(reg);
  std::cout << "woohoo large volume is " << volData.calculateSizeInBytes() << " bytes" << std::endl;
  //PolyVox::LargeVolume<uint8_t> volData(reg);
  //createSphereInVolume(volData, 64);
  //createFloorInVolume(volData, 12);
  std::cout << "woohoo populating volume from heightmap with scale " << (center2 / map.getWidth()) << std::endl;
  createHeightmapInVolume(volData, map, center2 / map.getWidth());

  m_camera->setPosition(Ogre::Vector3(center, center, center));

  std::cout << "woohoo smoothing volume" << std::endl;
  // Smooth
  PolyVox::RawVolume<uint8_t> tempVolume(volData.getEnclosingRegion());
  PolyVox::LowPassFilter< PolyVox::LargeVolume<uint8_t>, PolyVox::RawVolume<uint8_t>, int16_t > pass1(&volData, PolyVox::Region(volData.getEnclosingRegion()), &tempVolume, PolyVox::Region(volData.getEnclosingRegion()), 3);
  pass1.executeSAT();
  PolyVox::LowPassFilter< PolyVox::RawVolume<uint8_t>, PolyVox::LargeVolume<uint8_t>, int16_t > pass2(&tempVolume, PolyVox::Region(volData.getEnclosingRegion()), &volData, PolyVox::Region(volData.getEnclosingRegion()), 3);
  pass2.executeSAT();

  PolyVox::SurfaceMesh<PolyVox::PositionMaterialNormal> mesh;

  std::cout << "woohoo creating extractor" << std::endl;
  PolyVox::MarchingCubesSurfaceExtractor< PolyVox::LargeVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);
  //PolyVox::CubicSurfaceExtractorWithNormals< PolyVox::LargeVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &mesh);

  std::cout << "woohoo extracting into mesh" << std::endl;
  surfaceExtractor.execute();

  // Set the scene's ambient light
  m_scene_mgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

  std::cout << "woohoo creating mesh with " << mesh.getNoOfVertices() << " verts" << std::endl;

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
    //manual->colour(
    //    (rand() % 100) / 100.0f,
    //    (rand() % 100) / 100.0f,
    //    (rand() % 100) / 100.0f);
  }

  //Now we iterate through all the indices from the mesh and also add them to the ManualObject section
  for(std::vector<uint32_t>::const_iterator it = mesh.getIndices().begin(); it != mesh.getIndices().end(); ++it)
  {
    manual->index(*it);
  }

  //End the section then add the ManualObject to the scenegraph
  manual->end();
  m_scene_mgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

  std::cout << "woohoo initializing lighting" << std::endl;

  // Create a Light and set its position
  Ogre::Light* light = m_scene_mgr->createLight("MainLight");
  light->setPosition(20.0f, 80.0f, 50.0f);
}
