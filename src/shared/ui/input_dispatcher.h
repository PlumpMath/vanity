/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: shared/ui/input_dispatcher.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Singleton that hands input events to all
    alive windows.
*/

#pragma once

#include <functional>
#include <vector>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

namespace ui
{
  class window;

  class input_dispatcher
  {
    public:
      /* Not copyable/movable. */
      input_dispatcher(input_dispatcher const &) = delete;
      input_dispatcher(input_dispatcher &&) = delete;
      input_dispatcher& operator =(input_dispatcher const &) = delete;
      input_dispatcher& operator =(input_dispatcher &&) = delete;

      /* Singleton access. */
      static input_dispatcher& global()
      { static input_dispatcher id; return id; }

      /* Both add/remove must be done exactly once per window. */
      void add_window(window &win);
      void remove_window(window &win);

      void key_pressed(OIS::KeyEvent const &arg);
      void key_released(OIS::KeyEvent const &arg);
      void mouse_moved(OIS::MouseEvent const &arg);
      void mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id);
      void mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id);

    private:
      input_dispatcher() = default;

      std::vector<std::reference_wrapper<window>> m_windows;
  };
}
