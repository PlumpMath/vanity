/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: shared/ui/input_dispatcher.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "input_dispatcher.h"
#include "window.h"

namespace ui
{
  void input_dispatcher::add_window(window &win)
  {
    if(std::find(m_windows.begin(), m_windows.end(), std::ref(win))
                 == m_windows.end())
    { m_windows.push_back(win); }
    else
    { throw std::invalid_argument("Window already exists"); }
  }

  void input_dispatcher::remove_window(window &win)
  {
    auto const it(std::find(m_windows.begin(), m_windows.end(), std::ref(win)));
    if(it != m_windows.end())
    { m_windows.erase(it); }
    else
    { throw std::invalid_argument("Window does not exist"); }
  }

  void input_dispatcher::key_pressed(OIS::KeyEvent const &arg)
  {
    for(auto &win : m_windows)
    { win.get().key_pressed(arg); }
  }

  void input_dispatcher::key_released(OIS::KeyEvent const &arg)
  {
    for(auto &win : m_windows)
    { win.get().key_released(arg); }
  }

  void input_dispatcher::mouse_moved(OIS::MouseEvent const &arg)
  {
    for(auto &win : m_windows)
    { win.get().mouse_moved(arg); }
  }

  void input_dispatcher::mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  {
    for(auto &win : m_windows)
    { win.get().mouse_pressed(arg, id); }
  }

  void input_dispatcher::mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  {
    for(auto &win : m_windows)
    { win.get().mouse_released(arg, id); }
  }
}
