/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/source/finite.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps a source and provides a finite
    (non-streamed) buffer in which audio
    is stored. Finite buffers, once loaded,
    can be replayed over and over.
*/

#pragma once

#include "base.h"
#include "../buffer.h"

namespace audio
{
  namespace source
  {
    class finite
    {
      public:
        finite() = default;
        finite(finite const&) = delete;
        finite(finite &&) = delete;

        finite& operator =(finite const&) = delete;
        finite& operator =(finite &&) = delete;

        void play();
        void resume();
        void pause();
        void stop();

        bool is_playing()
        { return m_base.is_playing(); }

        template <format F>
        void set_buffer(buffer<F> const * const buf)
        { m_base.set(base::property::buffer, buf->get_buffer()); }
        void set_buffer(std::nullptr_t)
        { m_base.set(base::property::buffer, 0); }

        state::type get_state() const
        { return m_state; }

      private:
        void set_state(state::type const s)
        { m_state = s; }

        base m_base;
        state::type m_state{ state::stopped };
    };
  }
}
