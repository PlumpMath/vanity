/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/file.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Provides finite buffer (non-stream) loading of
    Ogg Vorbis files. The loading is not streamed
    and happens entirely up front, synchronously.
    Once a file is loaded, it may be played and
    replayed over and over; the contents stay
    in memory.
*/

#pragma once

#include "common.h"
#include "../source/finite.h"

namespace audio
{
  namespace ogg
  {
    class file
    {
      public:
        file() = default;
        file(std::string const &path)
        { load(path); }
        file(file const&) = delete;
        file(file &&) = delete;

        file& operator =(file const&) = delete;
        file& operator =(file &&) = delete;

        void load(std::string const &path);

        void play();
        void resume();
        void pause();
        void stop();

        bool is_playing();

        state::type get_state() const
        { return m_state; }

      private:
        void set_state(state::type const s)
        { m_state = s; }

        state::type m_state{ state::stopped };
        buffer<format::mono16> m_buffer;
        source::finite m_source;
        std::unique_ptr<detail::file> m_file;
        std::string m_filename;

        static size_t constexpr const m_buff_size{ 4096 };
    };
  }
}
