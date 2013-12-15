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
#include <future>

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

        auto const size(m_region.get_width());
        m_data.resize(size);

        std::mutex loaded_mutex;
        size_t loaded{};
        fill_region(func, 0, size, [&](size_t const chunk)
        {
          std::lock_guard<std::mutex> guard(loaded_mutex);
          loaded += chunk;
          std::cout << "loaded " << (loaded * 100.0f / size) << "%" << std::endl;
        });

        std::cout << "volume filled" << std::endl;
      }

      void fill_region(fill_func_t const &func,
                       size_t const start_x, size_t const end_x,
                       std::function<void (size_t const)> const &report)
      {
        if(start_x == end_x)
        { return; }

        /* Clamp between 64 and 256, but aim for whatever suits 8 threads. */
        auto const weighted(std::max<size_t>(64, std::min<size_t>(256, (end_x - start_x) / 8)));
        auto const width(std::min<size_t>(weighted, (end_x - start_x)));

        auto fut(std::async(std::launch::async, 
                            std::bind(&fixed_volume<Value>::fill_region,
                                      this, func, start_x + width, end_x, report )));

        for(size_t x{ start_x }; x < width + start_x; ++x)
        {
          m_data[x].resize(m_region.get_height());
          for(size_t y{}; y < m_region.get_height(); ++y)
          {
            m_data[x][y].resize(m_region.get_depth());
            for(size_t z{}; z < m_region.get_depth(); ++z)
            { m_data[x][y][z] = func({ x, y, z }); }
          }
        }
        report(width);
      }

      container_t m_data;
      region const m_region;
  };
}
