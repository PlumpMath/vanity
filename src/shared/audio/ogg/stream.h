/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/stream.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Provides streaming loading of Ogg Vorbis
    files from disk. Once files contents have
    been played, they are discarded and the
    stream (file) must be opened again to replay.
*/

#pragma once

#include "common.h"
#include "../source/streaming.h"

namespace audio
{
  namespace ogg
  {
    class stream
    {
      public:
        stream()
          : m_buffers(m_buffer_count)
        { }
        stream(std::string const &path)
          : m_buffers(m_buffer_count)
        { load(path); }
        stream(stream const&) = delete;
        stream(stream &&) = delete;

        stream& operator =(stream const&) = delete;
        stream& operator =(stream &&) = delete;

        void load(std::string const &path);

        void play();
        void resume();
        void pause();
        void stop();

        void process();

        bool is_playing() const
        { return get_state() == state::playing; }

        state::type get_state() const
        { return m_state; }

      private:
        void set_state(state::type const s)
        { m_state = s; }

        state::type m_state{ state::stopped };
        buffer_queue<format::mono16> m_buffers;
        source::streaming m_source;
        std::unique_ptr<detail::file> m_file;
        std::string m_filename;

        static size_t constexpr m_buffer_count{ 4 };
        static size_t constexpr m_buff_size{ 4096 };
    };
  }
}
