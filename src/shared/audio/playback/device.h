/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/playback/device.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wrapper around an OpenAL playback device
    which allows for mapping of output to a
    hardware device.
*/

#pragma once

#include <AL/al.h> 
#include <AL/alc.h> 

#include <string>
#include <vector>
#include <memory>

namespace audio
{
  namespace playback
  {
    class device
    {
      public:
        using name_t = std::string;
        using device_t = ALCdevice;

        device() = delete;
        device(name_t const &name);
        device(device const&) = delete;
        device(device &&) = default;

        device& operator =(device const&) = delete;
        device& operator =(device &&) = default;

        device_t* get_device()
        { return m_device.get(); }

        ALint* get_attributes()
        { return nullptr; }

      private:
        std::unique_ptr<device_t, decltype(&alcCloseDevice)> m_device;
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
