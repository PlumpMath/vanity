/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/source/streaming.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "streaming.h"

namespace audio
{
  namespace source
  {
    void streaming::play()
    {
      state::assert(*this, state::stopped);
      set_state(state::playing);

      m_base.play();
    }

    void streaming::resume()
    {
      state::assert(*this, state::paused);
      set_state(state::playing);

      m_base.resume();
    }

    void streaming::pause()
    {
      state::assert(*this, state::playing); 
      set_state(state::paused);

      m_base.pause();
    }

    void streaming::stop()
    {
      state::assert(*this, state::playing, state::paused);
      set_state(state::stopped);

      m_base.stop();

      /* Clear up our queuing. */
      ALint out{};
      alCheck(alGetSourcei(m_base.get_source(), AL_BUFFERS_PROCESSED, &out));
      if(out)
      {
        std::vector<ALuint> ubufs(out);
        alCheck(alSourceUnqueueBuffers(m_base.get_source(), out, ubufs.data()));
      }
    }
  }
}
