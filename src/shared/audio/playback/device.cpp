/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/playback/device.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "device.h"

#include <cstring>

namespace audio
{
  namespace playback
  {
    device::device(name_t const &name)
      : m_device(nullptr, &alcCloseDevice)
    {
      m_device.reset(alcOpenDevice(name.c_str()));
      if(!m_device)
      { throw std::runtime_error("Unable to open device: " + name); }
    }

    devices::devices()
    {
      auto const *devices(alcGetString(nullptr, ALC_DEVICE_SPECIFIER));
      while(*devices)
      { 
        m_devices.push_back(devices);
        devices += std::strlen(devices) + 1; 
      } 
    }
  }
}
