/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/source/streaming.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Composes a source and offers services for queuing
    and dequeing buffers. This should be used with
    a buffer_queue. The streaming data could come from
    disk files, network streams, et cetera.
*/

#pragma once

#include "base.h"
#include "../buffer.h"

namespace audio
{
  namespace source
  {
    class streaming
    {
      public:
        streaming() = default;
        streaming(streaming const&) = delete;
        streaming(streaming &&) = delete;

        streaming& operator =(streaming const&) = delete;
        streaming& operator =(streaming &&) = delete;

        void play();
        void resume();
        void pause();
        void stop();

        bool is_playing()
        { return m_base.is_playing(); }

        template <format F>
        size_t unqueue_buffers(buffer_queue<F> &bufs)
        {
          state::assert(*this, state::playing); 

          /* Poll for recoverable buffers. */
          ALint out{};
          alCheck(alGetSourcei(m_base.get_source(), AL_BUFFERS_PROCESSED, &out));
          if(out)
          {
            std::vector<ALuint> ubufs(out);
            alCheck(alSourceUnqueueBuffers(m_base.get_source(), out, ubufs.data()));
            for(ALint i{}; i < out; ++i)
            {
              bufs.processed[i].get_data().clear();
              bufs.processing.push_back(std::move(bufs.processed[i]));
              bufs.processed.erase(bufs.processed.begin() + i);
            }
          }

          return out;
        }

        template <format F>
        size_t queue_buffers(buffer_queue<F> &bufs)
        {
          state::assert(*this, state::playing); 

          size_t queued{};
          for(size_t i{}; i < bufs.processing.size(); ++i)
          {
            if(bufs.processing[i].get_data().size())
            {
              auto const name(bufs.processing[i].get_buffer());
              alCheck(alSourceQueueBuffers(m_base.get_source(), 1, &name));
              bufs.processed.push_back(std::move(bufs.processing[i]));
              bufs.processing.erase(bufs.processing.begin() + i);
              ++queued;
            }
          }

          /* Taking too long to process buffers could dry up
           * the queue, causing the source to stop playing. */
          if(!is_playing())
          { set_state(state::stopped); play(); }

          return queued;
        }

        state::type get_state() const
        { return m_state; }
        base::source_t get_source() const
        { return m_base.get_source(); }

      private:
        void set_state(state::type const s)
        { m_state = s; }

        base m_base;
        state::type m_state{ state::stopped };
    };
  }
}
