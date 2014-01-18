/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/capture/device.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper around an OpenAL capture device
    which allows for mapping of input to a
    hardware device.
*/

#pragma once

#include "../buffer.h"
#include "../source/state.h"

#include <chrono>

namespace audio
{
  namespace capture
  {
    class device
    {
      public:
        using name_t = std::string;
        using device_t = ALCdevice;

        device() = delete;
        device(name_t const &name, ALuint const freq,
               format const fmt, size_t const size);
        device(device const&) = delete;
        device(device &&) = default;

        device& operator =(device const&) = delete;
        device& operator =(device &&) = default;

        void start();
        void stop();

        template <format F>
        typename buffer<F>::size_type process(buffer<F> &buf)
        {
          state::assert(*this, state::playing);

          auto const samples(get_captured_samples());
          if(!samples)
          { return samples; }
          if(samples < m_max_samples)
          { return 0; }

          /* Make enough space. */
          auto &data(buf.get_data());
          auto const old_size(data.size());
          data.resize(data.size() + (samples * buffer<F>::sample_size));

          alcCaptureSamples(m_device.get(), data.data() + old_size, samples); 
          alcCheck(m_device.get());

          return samples;
        }

        template <format F>
        typename buffer<F>::size_type process(buffer_queue<F> &buf)
        {
          state::assert(*this, state::playing);

          for(size_t i{}; i < buf.processing.size(); ++i)
          {
            if(!buf.processing[i].get_data().size())
            {
              auto const samples(process(buf.processing[i]));
              if(samples)
              { buf.processing[i].upload(); }
              return samples;
            }
          }

          return 0;
        }

        device_t* get_device()
        { return m_device.get(); }
        ALint* get_attributes()
        { return nullptr; }

        state::type get_state() const
        { return m_state; }

      private:
        ALint get_captured_samples() const;

        void set_state(state::type const s)
        { m_state = s; }

        state::type m_state{ state::stopped };
        std::unique_ptr<device_t, decltype(&alcCaptureCloseDevice)> m_device;

        /* The amount of samples to capture before spilling into a buffer. */
        static ALuint constexpr m_max_samples{ 2048 }; 
    };

    class devices
    {
      public:
        using container_t = std::vector<device::name_t>;
        using const_iterator = container_t::const_iterator;

        devices();

        const_iterator begin() const
        { return m_devices.begin(); }
        const_iterator cbegin() const
        { return m_devices.begin(); }
        const_iterator end() const
        { return m_devices.end(); }
        const_iterator cend() const
        { return m_devices.end(); }

        device::name_t const& operator [](size_t const index) const
        { return m_devices[index]; }

        size_t size() const
        { return m_devices.size(); }

      private:
        container_t m_devices;
    };
  }
}
