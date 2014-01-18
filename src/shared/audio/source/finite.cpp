/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/source/finite.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "finite.h"

namespace audio
{
  namespace source
  {
    void finite::play()
    {
      state::assert(*this, state::stopped);
      set_state(state::playing);

      m_base.play();
    }

    void finite::resume()
    {
      state::assert(*this, state::paused);
      set_state(state::playing);

      m_base.resume();
    }

    void finite::pause()
    {
      state::assert(*this, state::playing); 
      set_state(state::paused);

      m_base.pause();
    }

    void finite::stop()
    {
      state::assert(*this, state::playing, state::paused);
      set_state(state::stopped);

      m_base.stop();
    }
  }
}
