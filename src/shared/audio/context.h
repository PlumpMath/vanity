/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/context.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper around an OpenAL context which
    houses and offers means of accessing buffers,
    devices, sources, et cetera. There may be multiple
    contexts in an application at once, but only one
    made 'current' at once. Contexts are limited to
    operate solely on an owned thread (though multiple
    threads may each have a separate context). Context
    'currency' is thread-local.
*/

#pragma once

#include <AL/al.h> 
#include <AL/alc.h> 

#include "playback/device.h"
#include "check.h"

namespace audio
{
  class context
  {
    public:
      context() = delete;
      context(playback::device &dev)
        : m_context(nullptr, &alcDestroyContext), m_device(dev)
      {
        m_context.reset(alcCreateContext(dev.get_device(), dev.get_attributes()));
        if(!m_context)
        { throw std::runtime_error("Unable to open context"); }
      }
      context(context const&) = delete;
      context(context &&) = default;
      ~context()
      { alcMakeContextCurrent(nullptr); }

      context& operator =(context const&) = delete;
      context& operator =(context &&) = default;

      void make_current()
      {
        alcMakeContextCurrent(m_context.get()); 
        alcCheck(m_device.get_device());
        alcProcessContext(m_context.get()); 
        alcCheck(m_device.get_device());
      }

    private:
      std::unique_ptr<ALCcontext, decltype(&alcDestroyContext)> m_context;
      playback::device &m_device;
  };
}
