/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/base.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper around an OpenAL source that
    plays audio from buffer(s). This is not
    a base in the inheritance sense; this
    should be contained by more defined sources.
    (like finite and streaming)
*/

#pragma once

#include <AL/alc.h> 

#include <string>
#include <memory>
#include <type_traits>

#include "state.h"
#include "../check.h"

namespace audio
{
  namespace source
  {
    /* This is meant to be composed (not inherited).
     * It does not implement error checking around those
     * functions which should be forwarded. Add it if you need. */
    class base
    { 
      public:
        using source_t = ALuint;

        enum class property : ALenum
        {
          relative = AL_SOURCE_RELATIVE,
          cone_inner_angle = AL_CONE_INNER_ANGLE,
          cone_outer_angle = AL_CONE_OUTER_ANGLE,
          looping = AL_LOOPING,
          buffer = AL_BUFFER,
          state = AL_SOURCE_STATE,
          buffers_queued = AL_BUFFERS_QUEUED,
          buffers_processed = AL_BUFFERS_PROCESSED
        };

        base()
          : m_source(new source_t{}, &base::delete_source)
        { alCheck(alGenSources(1, m_source.get())); }
        base(base const&) = delete;
        base(base &&) = default;

        base& operator =(base const&) = delete;
        base& operator =(base &&) = default;

        void play()
        { alCheck(alSourcePlay(*m_source)); }
        void resume()
        { alCheck(alSourcePlay(*m_source)); }
        void pause()
        { alCheck(alSourcePause(*m_source)); }
        void stop()
        { alCheck(alSourceStop(*m_source)); }

        bool is_playing() const
        { return get<state::type>(property::state) == state::playing; }

        /* Property mutators. */
        template <typename T>
        void set(property const p, T const t,
            typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type* =0)
        { alCheck(alSourcei(*m_source, static_cast<std::underlying_type<property>::type>(p), t)); }
        template <typename T>
        void set(property const p, T const t,
            typename std::enable_if<std::is_floating_point<T>::value>::type* =0)
        { alCheck(alSourcef(*m_source, static_cast<std::underlying_type<property>::type>(p), t)); }

        /* Property accessors. */
        template <typename T>
        T get(property const p,
            typename std::enable_if<std::is_integral<T>::value || std::is_enum<T>::value>::type* =0) const
        {
          ALint out{};
          alCheck(alGetSourcei(*m_source, static_cast<std::underlying_type<base::property>::type>(p), &out));
          return static_cast<T>(out);
        }
        template <typename T>
        T get(property const p,
            typename std::enable_if<std::is_floating_point<T>::value>::type* =0) const
        {
          ALfloat out{};
          alCheck(alGetSourcef(*m_source, static_cast<std::underlying_type<base::property>::type>(p), &out));
          return static_cast<T>(out);
        }

        source_t get_source() const
        { return *m_source; }

      private:
        static void delete_source(source_t const * const so)
        { alCheck(alDeleteSources(1, so)); }

        std::unique_ptr<source_t, decltype(&base::delete_source)> m_source;
    };
  }

  namespace state
  {
    template <>
    inline std::string to_string<source::base::property>(source::base::property const p)
    {
      switch(p)
      {
        case source::base::property::relative:
          return "Relative";
        case source::base::property::cone_inner_angle:
          return "Cone Inner Angle";
        case source::base::property::cone_outer_angle:
          return "Cone Outer Angle";
        case source::base::property::looping:
          return "Looping";
        case source::base::property::buffer:
          return "Buffer";
        case source::base::property::state:
          return "State";
        case source::base::property::buffers_queued:
          return "Buffers Queued";
        case source::base::property::buffers_processed:
          return "Buffers Processed";
        default:
          throw std::invalid_argument("Invalid property: " +
              std::to_string(static_cast<std::underlying_type<source::base::property>::type>(p)));
      }
    }
  }
}
