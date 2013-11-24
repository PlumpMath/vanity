/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: vox/fixed_volume.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <vector>
#include <functional>

#include "region.h"

namespace vox
{
  template <typename Value>
  class fixed_volume
  {
    public:
      using container_t = std::vector<std::vector<std::vector<Value>>>;
      using container_index_t = std::vector<std::vector<Value>>;
      using fill_func_t = std::function<Value (vec3<size_t> const&)>;
      using value_t = Value;

      fixed_volume(region const &size)
        : m_region(size)
      { fill([](vec3<size_t> const&){ return Value{}; }); }

      fixed_volume(region const &size, fill_func_t const &func)
        : m_region(size)
      { fill(func); }

      Value& at(size_t const x, size_t const y, size_t const z)
      { return m_data.at(x).at(y).at(z); }
      Value const& at(size_t const x, size_t const y, size_t const z) const
      { return m_data.at(x).at(y).at(z); }

      container_index_t& operator [](size_t const index)
      { return m_data[index]; }
      container_index_t const& operator [](size_t const index) const
      { return m_data[index]; }

      region const& get_region() const
      { return m_region; }

    private:
      void fill(fill_func_t const &func)
      {
        std::cout << "filling volume" << std::endl;
        m_data.resize(m_region.get_width());
        for(size_t x{}; x < m_region.get_width(); ++x)
        {
          if(x % (m_region.get_width() / 10) == 0)
          { std::cout << "  " << (x * 100.0f / m_region.get_width()) << "%" << std::endl; }

          m_data[x].resize(m_region.get_height());
          for(size_t y{}; y < m_region.get_height(); ++y)
          {
            m_data[x][y].resize(m_region.get_depth());
            for(size_t z{}; z < m_region.get_depth(); ++z)
            { m_data[x][y][z] = func({ x, y, z }); }
          }
        }
        std::cout << "volume filled" << std::endl;
      }

      container_t m_data;
      region const m_region;
  };
}
