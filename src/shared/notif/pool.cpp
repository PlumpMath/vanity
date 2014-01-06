/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: notif/pool.cpp
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#include "pool.h"

namespace notif
{
  pool& pool::get()
  {
    static pool ep;
    return ep;
  }

  bool pool::poll()
  {
    func_t event;

    { /* No lock is needed while running the event; add some scope. */
      std::lock_guard<std::mutex> const lock(m_events_lock);

      if(m_events.empty())
      { return false; }

      event = m_events.front();
      m_events.pop();
    }

    event();
    return true;
  }
}
