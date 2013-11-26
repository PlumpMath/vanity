/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/surface_extractor.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <iostream>
#include <cassert>
#include <limits>

#include "tables.h"
#include "region.h"
#include "surface.h"
#include "grid_cell.h"

namespace vox
{
  template <typename Triangle, typename Volume>
  class surface_extractor
  {
    public:
      using this_t = surface_extractor<Triangle, Volume>;
      using surface_t = surface<Triangle>;
      using value_t = typename Volume::value_t;

      surface_extractor(Volume const &vol, region const &reg,
                        value_t const level, size_t const unit)
        : m_volume(vol)
        , m_region(reg)
        , m_iso_level(level)
        , m_unit_size(unit)
      { validate(); }
      surface_extractor(this_t const &se)
        : m_volume(se.m_volume)
        , m_region(se.m_region)
        , m_iso_level(se.m_iso_level)
        , m_unit_size(se.m_unit_size)
      { validate(); }

      this_t& operator =(this_t const &) = delete;

      region const& get_region() const
      { return m_region; }

      surface_t operator ()() const
      {
        surface_t surface(m_region);
        std::vector<Triangle> triangles;
        grid_cell<value_t> grid;

        for(size_t x{}; x < m_region.get_width() - m_unit_size; x += m_unit_size)
        {
          for(size_t y{}; y < m_region.get_height() - m_unit_size; y += m_unit_size)
          {
            for(size_t z{}; z < m_region.get_depth() - m_unit_size; z += m_unit_size)
            {
              grid.p[0].x = x;
              grid.p[0].y = y;
              grid.p[0].z = z;
              grid.val[0] = m_volume[x][y][z];

              grid.p[1].x = x + m_unit_size;
              grid.p[1].y = y;
              grid.p[1].z = z; 
              grid.val[1] = m_volume[x + m_unit_size][y][z];

              grid.p[2].x = x + m_unit_size;
              grid.p[2].y = y + m_unit_size;
              grid.p[2].z = z;
              grid.val[2] = m_volume[x + m_unit_size][y + m_unit_size][z];

              grid.p[3].x = x;
              grid.p[3].y = y + m_unit_size;
              grid.p[3].z = z;
              grid.val[3] = m_volume[x][y + m_unit_size][z];

              grid.p[4].x = x;
              grid.p[4].y = y;
              grid.p[4].z = z + m_unit_size;
              grid.val[4] = m_volume[x][y][z + m_unit_size];

              grid.p[5].x = x + m_unit_size;
              grid.p[5].y = y;
              grid.p[5].z = z + m_unit_size;
              grid.val[5] = m_volume[x + m_unit_size][y][z + m_unit_size];

              grid.p[6].x = x + m_unit_size;
              grid.p[6].y = y + m_unit_size;
              grid.p[6].z = z + m_unit_size;
              grid.val[6] = m_volume[x + m_unit_size][y + m_unit_size][z + m_unit_size];

              grid.p[7].x = x;
              grid.p[7].y = y + m_unit_size;
              grid.p[7].z = z + m_unit_size;
              grid.val[7] = m_volume[x][y + m_unit_size][z + m_unit_size];

              std::vector<Triangle> const tris{ polygonize(grid) };
              surface.template add_triangles(tris.cbegin(), tris.cend());
            }
          }
        }

        return surface;
      }

    private:
      void validate() const
      {
        assert(m_volume.get_region().contains(m_region));
      }

      /*
         Given a grid cell and an isolevel, calculate the triangular
         facets requied to represent the isosurface through the cell.
         Return the number of triangular facets, the array "triangles"
         will be loaded up with the vertices at most 5 triangular facets.
         0 will be returned if the grid cell is either totally above
         of totally below the isolevel.
         */
      std::vector<Triangle> polygonize(grid_cell<value_t> const g) const
      {
        /* Determine the index into the edge table, which
           tells us the vertices inside of the surface. */
        int32_t cube_index{};
        for(size_t i{}; i < 8; ++i)
        {
          if(g.val[i] < m_iso_level)
          { cube_index |= (1 << i); }
        }

        /* Cube is entirely in/out of the surface */
        if(edge_table[cube_index] == 0)
        { return std::vector<Triangle>(); }

        /* Find the vertices where the surface intersects the cube */
        vec3<float> verts[12];
        if(edge_table[cube_index] & 1) 
        { verts[0] = interp(g.p[0], g.p[1], g.val[0], g.val[1]); }
        if(edge_table[cube_index] & 2) 
        { verts[1] = interp(g.p[1], g.p[2], g.val[1], g.val[2]); }
        if(edge_table[cube_index] & 4) 
        { verts[2] = interp(g.p[2], g.p[3], g.val[2], g.val[3]); }
        if(edge_table[cube_index] & 8) 
        { verts[3] = interp(g.p[3], g.p[0], g.val[3], g.val[0]); }
        if(edge_table[cube_index] & 16) 
        { verts[4] = interp(g.p[4], g.p[5], g.val[4], g.val[5]); }
        if(edge_table[cube_index] & 32) 
        { verts[5] = interp(g.p[5], g.p[6], g.val[5], g.val[6]); }
        if(edge_table[cube_index] & 64) 
        { verts[6] = interp(g.p[6], g.p[7], g.val[6], g.val[7]); }
        if(edge_table[cube_index] & 128) 
        { verts[7] = interp(g.p[7], g.p[4], g.val[7], g.val[4]); }
        if(edge_table[cube_index] & 256) 
        { verts[8] = interp(g.p[0], g.p[4], g.val[0], g.val[4]); }
        if(edge_table[cube_index] & 512) 
        { verts[9] = interp(g.p[1], g.p[5], g.val[1], g.val[5]); }
        if(edge_table[cube_index] & 1024) 
        { verts[10] = interp(g.p[2], g.p[6], g.val[2], g.val[6]); }
        if(edge_table[cube_index] & 2048) 
        { verts[11] = interp(g.p[3], g.p[7], g.val[3], g.val[7]); }

        /* Create the triangles */
        std::vector<Triangle> triangles;
        triangles.reserve(5);
        for(size_t i{}; tri_table[cube_index][i] != -1; i += 3)
        {
          triangles.push_back(Triangle(
              verts[tri_table[cube_index][i]],
              verts[tri_table[cube_index][i + 1]],
              verts[tri_table[cube_index][i + 2]] ));
        }

        return triangles;
      }

      vec3<float> interp(vec3<float> const &p1, vec3<float> const &p2, float valp1, float valp2) const
      {
        float constexpr ep{ std::numeric_limits<float>::epsilon() };

        if(std::abs(m_iso_level - valp1) < ep)
        { return p1; }
        if(std::abs(m_iso_level - valp2) < ep)
        { return p2; }
        if(std::abs(valp1 - valp2) < ep)
        { return p1; }

        float const mu{ (m_iso_level - valp1) / (valp2 - valp1) };
        return
        { p1.x + mu * (p2.x - p1.x),
          p1.y + mu * (p2.y - p1.y),
          p1.z + mu * (p2.z - p1.z) };
      }


      Volume const &m_volume;
      region const m_region;
      value_t const m_iso_level;
      size_t const m_unit_size;
  };
}
