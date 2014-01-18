/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/state.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Types and procedures for maintaining
    audio state. Used by multiple objects
    within the audio system.
*/

#pragma once

#include <AL/al.h> 

#include <string>
#include <stdexcept>

namespace audio
{
  namespace state
  {
    enum values : ALenum
    {
      initial = AL_INITIAL,
      playing = AL_PLAYING,
      paused = AL_PAUSED,
      stopped = AL_STOPPED
    };
    using type = values;

    template <typename T>
    std::string to_string(T const);
    template <typename T, typename... States>
    inline void assert(T const &t, States... states)
    {
      auto const t_state(t.get_state());
      bool valid{ false };
      int const dummy[]{ (valid |= (t_state == states), 0)... };
      static_cast<void>(dummy);

      if(!valid)
      {
        std::string error("Invalid state ('" + to_string(t_state) + "' is none of:");
        int const dummy[]{ (error += " '" + to_string(states) + "'", 0)... };
        static_cast<void>(dummy);
        throw std::runtime_error(error + ")");
      }
    }

    template <>
    inline std::string to_string<state::type>(state::type const s)
    {
      switch(s)
      {
        case state::initial:
          return "Initial";
        case state::playing:
          return "Playing";
        case state::paused:
          return "Paused";
        case state::stopped:
          return "Stopped";
        default:
          throw std::invalid_argument("Invalid state: " +
              std::to_string(static_cast<state::type>(s)));
      }
    }
  }
}
