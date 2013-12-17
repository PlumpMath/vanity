/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/surface_factory.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include "surface.h"

namespace ui
{
  class surface_factory : public Awesomium::SurfaceFactory
  {
    public:
      Awesomium::Surface* CreateSurface(Awesomium::WebView *view, int width, int height) override
      {
        surface * const s{ new surface(width, height) };
        return s;
      }

      void DestroySurface(Awesomium::Surface *sur) override
      {
        delete sur;
        sur = nullptr;
      }
  };
}
