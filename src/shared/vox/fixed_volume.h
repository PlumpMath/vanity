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
#include "log/logger.h"

namespace vox
{
  template <typename Value>
  class fixed_volume
  {
    public:
      using value_t = Value;
      using container_t = std::vector<std::vector<std::vector<value_t>>>;
      using container_index_t = std::vector<std::vector<value_t>>;
      using fill_func_t = std::function<void (fixed_volume&, size_t const, size_t const)>;

      fixed_volume(region const &size)
        : m_region(size)
      { fill([](size_t const, size_t const){ return value_t{}; }); }

      fixed_volume(region const &size, fill_func_t const &func)
        : m_region(size)
      { fill(func); }

      value_t& at(size_t const x, size_t const y, size_t const z)
      { return m_data.at(x).at(y).at(z); }
      value_t const& at(size_t const x, size_t const y, size_t const z) const
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
        log_info("filling volume");
        log_push();

        auto const size(m_region.get_width());
        m_data.resize(size);

        std::mutex loaded_mutex;
        size_t loaded{};
        auto const report([&](size_t const chunk)
        {
          std::lock_guard<std::mutex> guard(loaded_mutex);
          loaded += chunk;
          log_debug("loaded %%%", (loaded * 100.0f / size));
        });

        auto const width(size / m_max_threads);
        std::vector<std::future<void>> futs;
        futs.reserve(m_max_threads);
        for(size_t i{}; i < m_max_threads; ++i)
        {
          futs.push_back(std::async(std::launch::async,
                         std::bind(&fixed_volume<value_t>::fill_region, this,
                         func, i * width, (i * width) + width, report)));
        } futs.clear();

        log_pop();
        log_info("volume filled");
      }

      void fill_region(fill_func_t const &func,
                       size_t const start_x, size_t const end_x,
                       std::function<void (size_t const)> const &report)
      {
        if(start_x == end_x)
        { return; }

        auto const width(end_x - start_x);
        auto const region_height(m_region.get_height());
        auto const region_depth(m_region.get_depth());
        for(size_t x{ start_x }; x < width + start_x; ++x)
        {
          m_data[x].resize(region_height);
          for(size_t y{}; y < region_height; ++y)
          { m_data[x][y].resize(region_depth); }
        }

        static constexpr const size_t report_rate{ 64 };
        auto curr_x(start_x);
        while(curr_x != end_x)
        {
          auto const span_x(std::min(end_x - curr_x, report_rate));;
          func(*this, curr_x, curr_x + span_x);
          curr_x += span_x;
          report(span_x);
        }
      }

      container_t m_data;
      region const m_region;
      static constexpr const size_t m_max_threads{ 8 };
  };
}
