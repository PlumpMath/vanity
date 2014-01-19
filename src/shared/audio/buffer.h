/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/buffer.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps an OpenAL buffer, which is
    the only way to store audio data.
    Buffers can be filled and preserved
    (by a finite source) or queued, swapped,
    and dumped (by a streaming source).
*/

#pragma once

#include <vector>
#include <memory>

#include <AL/alc.h> 

#include "check.h"

namespace audio
{
  enum class format : ALenum
  {
    mono8 = AL_FORMAT_MONO8,
    mono16 = AL_FORMAT_MONO16,
    stereo8 = AL_FORMAT_STEREO8,
    stereo16 = AL_FORMAT_STEREO16
  };

  namespace detail
  {
    template <format F>
    struct sample_size
    { };
    template <>
    struct sample_size<format::mono8>
    {
      using type = uint8_t;
      static size_t constexpr const value{ 1 };
    };
    template <>
    struct sample_size<format::mono16>
    {
      using type = uint16_t;
      static size_t constexpr const value{ 2 };
    };
    template <>
    struct sample_size<format::stereo8>
    {
      using type = uint8_t;
      static size_t constexpr const value{ 1 }; 
    };
    template <>
    struct sample_size<format::stereo16>
    {
      using type = uint16_t;
      static size_t constexpr const value{ 2 };
    };
  }

  template <format F>
  class buffer
  {
    public:
      using buffer_t = ALuint;
      using size_type = ALCsizei;
      using data_t = ALubyte;
      using container_t = std::vector<data_t>;
      static format constexpr const format_value{ F };
      using sample_type = typename detail::sample_size<F>::type;
      static size_type constexpr const sample_size{ detail::sample_size<F>::value };

      buffer()
        : m_buffer(new buffer_t{}, &buffer::delete_buffer)
      { alCheck(alGenBuffers(1, m_buffer.get())); }
      buffer(buffer const &) = delete;
      buffer(buffer &&) = default;

      buffer& operator =(buffer &&) = default;
      buffer& operator =(buffer const &) = delete;

      buffer_t get_buffer() const
      { return *m_buffer; }
      container_t& get_data()
      { return m_data; }
      container_t const& get_data() const
      { return m_data; }

      void upload()
      { alCheck(alBufferData(*m_buffer, static_cast<typename std::underlying_type<format>::type>(F),
                     m_data.data(), m_data.size(), m_freq)); }

    private:
      static void delete_buffer(buffer_t const * const buf)
      { alCheck(alDeleteBuffers(1, buf)); }

      container_t m_data;
      std::unique_ptr<buffer_t, decltype(&buffer::delete_buffer)> m_buffer;
      static constexpr const ALuint m_freq{ 8000 }; /* TODO: Configure. */
  };

  template <format F>
  struct buffer_queue
  {
    static format constexpr const value = F;
    using data_t = typename buffer<F>::data_t;

    buffer_queue() = delete;
    buffer_queue(size_t const buffers)
      : processing(buffers)
    { }
    buffer_queue(buffer_queue const&) = delete;
    buffer_queue(buffer_queue &&) = default;

    buffer_queue& operator =(buffer_queue const&) = delete;
    buffer_queue& operator =(buffer_queue &&) = default;

    void clear()
    {
      for(size_t i{}; i < processed.size(); ++i)
      { processing.push_back(std::move(processed[i])); }
      processed.clear();

      for(auto &buf : processing)
      { buf.get_data().clear(); }
    }

    std::vector<buffer<F>> processing;
    std::vector<buffer<F>> processed;
  };
}
