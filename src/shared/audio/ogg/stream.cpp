/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/stream.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "stream.h"

namespace audio
{
  namespace ogg
  {
    void stream::load(std::string const &path)
    {
      audio::state::assert(*this, state::stopped);

      m_filename = path;
      m_file.reset(new detail::file(m_filename));
      if(m_file->get_format() != m_buffers.value)
      { throw std::runtime_error("Invalid ogg format"); }
    }

    void stream::play()
    {
      audio::state::assert(*this, state::stopped);

      /* Reopen the ogg file stream. */
      if(!m_file)
      {
        if(m_filename.size())
        { load(m_filename); }
        else
        { throw std::runtime_error("Not loaded"); }
      }
      set_state(state::playing);

      m_source.play();
    }

    void stream::resume()
    {
      audio::state::assert(*this, state::paused);
      set_state(state::playing);

      m_source.resume();
    }

    void stream::pause()
    {
      audio::state::assert(*this, state::playing); 
      set_state(state::paused);

      m_source.pause();
    }

    void stream::stop()
    {
      audio::state::assert(*this, state::playing, state::paused);
      set_state(state::stopped);

      m_source.stop();
      m_buffers.clear();
      m_file.reset();
    }

    void stream::process()
    {
      audio::state::assert(*this, state::playing); 

      m_source.unqueue_buffers(m_buffers);
      for(size_t i{}; i < m_buffers.processing.size(); ++i)
      {
        auto &buf(m_buffers.processing[i]);
        if(!buf.get_data().size())
        {
          auto bytes(m_file->read_bytes(m_buff_size));
          if(!bytes.size())
          {
            /* We failed to read from the stream. */
            stop();
            return;
          }

          /* Copy from the stream. */
          for(auto const byte : bytes)
          { buf.get_data().push_back(
              static_cast<decltype(m_buffers)::data_t>(byte)); }

          if(buf.get_data().size())
          { buf.upload(); }

          break;
        }
      }
      m_source.queue_buffers(m_buffers);
    }
  }
}
