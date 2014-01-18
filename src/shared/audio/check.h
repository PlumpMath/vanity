/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/check.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Offers error checking macros for the audio system
    and its subsystems. These should be used wherever
    possible.
*/

#pragma once

#include <AL/al.h> 
#include <AL/alc.h> 

#include <cstring>

#define alCheck(func) \
{ \
  func; \
  audio::check::al_work(#func, __FILE__, __LINE__); \
}
#define alcCheck(dev) \
{ \
  audio::check::alc_work(dev, __FILE__, __LINE__); \
}

namespace audio
{
  namespace check
  {
    void al_work(char const * const func, char const * const file, size_t const line);
    void alc_work(ALCdevice * const dev, char const * const file, size_t const line);
  }
}
