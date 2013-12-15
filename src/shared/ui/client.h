/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/client.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <cef_client.h>

#include "util/borrowed_ptr.h"
#include "listener.h"

namespace ui
{
  class client : public CefClient
  {
    public:
      client(borrowed_ptr<listener> l)
        : m_listener(l.get())
      { }

      CefRefPtr<CefRenderHandler> GetRenderHandler() override
      { return m_listener; }

    private:
      CefRefPtr<CefRenderHandler> m_listener;

    public:
      IMPLEMENT_REFCOUNTING(client)
  };
}
