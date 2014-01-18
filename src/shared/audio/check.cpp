/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/check.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "check.h"

#include <sstream>
#include <stdexcept>

namespace audio
{
  namespace check
  {
    struct openal_err
    { ALenum const code; };
    struct openalc_err
    { ALCenum const code; };

    static std::string to_string(openal_err const err)
    {
      switch(err.code)
      {
        case AL_INVALID_NAME:
          return "Invalid name";
        case AL_INVALID_ENUM:
          return "Invalid enum";
        case AL_INVALID_VALUE:
          return "Invalid value";
        case AL_INVALID_OPERATION:
          return "Invalid operation";
        case AL_OUT_OF_MEMORY:
          return "Out of memory";
        default:
          return "Unknown error";
      }
    }

    static std::string to_string(openalc_err const err)
    {
      switch(err.code)
      {
        case ALC_INVALID_ENUM:
          return "Invalid enum";
        case ALC_INVALID_VALUE:
          return "Invalid value";
        case ALC_OUT_OF_MEMORY:
          return "Out of memory";
        case ALC_INVALID_DEVICE:
          return "Invalid device";
        case ALC_INVALID_CONTEXT:
          return "Invalid context";
        default:
          return "Unknown error";
      }
    }

    void al_work(char const * const func, char const * const file, size_t const line)
    {
      openal_err const al_err{ ::alGetError() };
      if(al_err.code != AL_NO_ERROR)
      {
        std::stringstream ss;
        ss << "OpenAL error " << al_err.code << " (" << to_string(al_err) << ") at "
           << file << ":" << line << " - " << func;
        throw std::runtime_error(ss.str());
      }
    }

    void alc_work(ALCdevice * const dev, char const * const file, size_t const line)
    {
      openalc_err const alc_err{ ::alcGetError(dev) };
      if(alc_err.code != ALC_NO_ERROR)
      {
        std::stringstream ss;
        ss << "OpenALC error " << alc_err.code << " (" << to_string(alc_err) << ") at "
           << file << ":" << line;
        throw std::runtime_error(ss.str());
      }
    }
  }
}
