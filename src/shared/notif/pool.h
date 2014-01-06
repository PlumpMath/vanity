/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: notif/pool.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <string>
#include <utility>
#include <queue>
#include <thread>
#include <mutex>
#include <cstdint>

#include "dispatcher.h"

namespace notif
{
  class pool
  {
    public:
      using func_t = std::function<void ()>;

      static pool& get();

      template <typename T>
      void post(T const &event)
      {
        std::lock_guard<std::mutex> const lock(m_events_lock);
        m_events.push(std::bind(&dispatcher<T>::dispatch,
                                std::ref(dispatcher<T>::get()), event));
      }

      template <typename T>
      typename callback<T>::tag_t subscribe(typename callback<T>::func_t const &cb)
      {
        if(!cb)
        { throw std::invalid_argument("Invalid callback function"); }

        std::lock_guard<std::mutex> const lock(m_events_lock);
        return dispatcher<T>::get().subscribe(cb);
      }

      /* Returns whether or not an event was handled. */
      bool poll();

    private:
      std::queue<func_t> m_events;
      std::mutex m_events_lock;
  };
}
