/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/triangle.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <OgreVector3.h>

namespace vox
{
  template <typename Vertex>
  struct triangle
  {
    triangle() = default;
    triangle(Vertex const &v0, Vertex const &v1, Vertex const &v2)
      : verts({ v0, v1, v2})
    { calculate_normal(); }

    void calculate_normal()
    {
      Ogre::Vector3 const a{ verts[0].p.x - verts[1].p.x,
                             verts[0].p.y - verts[1].p.y,
                             verts[0].p.z - verts[1].p.z };
      Ogre::Vector3 const b{ verts[1].p.x - verts[2].p.x,
                             verts[1].p.y - verts[2].p.y,
                             verts[1].p.z - verts[2].p.z };
      normal = a.crossProduct(b);
      normal.normalise();
    }

    Vertex verts[3];
    Ogre::Vector3 normal{ 0.0f, 1.0f, 0.0f };
  };
}
