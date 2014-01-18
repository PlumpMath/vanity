/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/file.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "file.h"

namespace audio
{
  namespace ogg
  {
    void file::load(std::string const &path)
    {
      audio::state::assert(*this, state::stopped);

      m_filename = path;
      m_file.reset(new detail::file(path));
      if(m_file->get_format() != m_buffer.format_value)
      { throw std::runtime_error("Invalid ogg format"); }

      m_source.set_buffer(nullptr);

      m_buffer.get_data().clear();
      auto &buf_data(m_buffer.get_data());
      for(auto const byte : m_file->read_all())
      { buf_data.push_back(static_cast<decltype(m_buffer)::data_t>(byte)); }
      m_buffer.upload();

      m_source.set_buffer(&m_buffer);
    }

    void file::play()
    {
      audio::state::assert(*this, state::stopped);

      /* Reopen the ogg file. */
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

    void file::resume()
    {
      audio::state::assert(*this, state::paused);
      set_state(state::playing);

      m_source.resume();
    }

    void file::pause()
    {
      audio::state::assert(*this, state::playing); 
      set_state(state::paused);

      m_source.pause();
    }

    void file::stop()
    {
      audio::state::assert(*this, state::playing, state::paused);
      set_state(state::stopped);

      m_source.stop();
      m_file.reset();
    }

    bool file::is_playing()
    {
      auto const ip(m_source.is_playing());
      if(!ip && get_state() == state::playing)
      { stop(); }

      return ip;
    }
  }
}
