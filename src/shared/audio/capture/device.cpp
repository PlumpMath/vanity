/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/capture/device.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "device.h"

#include <cstring>

namespace audio
{
  namespace capture
  {
    device::device(name_t const &name, ALuint const freq,
                   format const fmt, size_t const size)
      : m_device(nullptr, &alcCaptureCloseDevice)
    {
      m_device.reset(alcCaptureOpenDevice(name.c_str(), freq,
            static_cast<std::underlying_type<format>::type>(fmt), size));
      if(!m_device)
      { throw std::runtime_error("Unable to open device: " + name); }
    }

    void device::start()
    {
      state::assert(*this, state::stopped);
      set_state(state::playing);

      alcCaptureStart(m_device.get());
      alcCheck(m_device.get());
    }

    void device::stop()
    {
      state::assert(*this, state::playing);
      set_state(state::stopped);

      alcCaptureStop(m_device.get());
      alcCheck(m_device.get());
    }

    ALint device::get_captured_samples() const
    {
      ALint out{};
      alcGetIntegerv(m_device.get(), ALC_CAPTURE_SAMPLES, 1, &out); 
      alcCheck(m_device.get());
      return out;
    }

    devices:: devices()
    {
      auto const *devices(alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER));
      while(*devices)
      { 
        m_devices.push_back(devices);
        devices += std::strlen(devices) + 1; 
      } 
    }
  }
}
