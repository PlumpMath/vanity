/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/surface.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <vector>

#include "region.h"

namespace vox
{
  template <typename Triangle>
  class surface
  {
    public:
      surface(region const &reg)
        : m_region(reg)
      { }
      surface(surface const &reg)
        : m_data(reg.m_data)
        , m_region(reg.m_region)
      { }
      surface(surface &&reg)
        : m_data(std::move(reg.m_data))
        , m_region(std::move(reg.m_region))
      { }
      surface<Triangle>& operator =(surface<Triangle> const &) = delete;

      template <typename It>
      void add_triangles(It begin, It const end)
      { m_data.insert(m_data.end(), begin, end); }

      std::vector<Triangle> const& get_triangles() const
      { return m_data; }

      region const& get_region() const
      { return m_region; }

    private:
      std::vector<Triangle> m_data;
      region const m_region;
  };
}
