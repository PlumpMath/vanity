/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/common.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Ogg Vorbis streaming and buffered loading
    both use the same OV decoding, so this is
    the commonality.
*/

#pragma once

#include <AL/al.h> 

#include <string>
#include <vector>

#include "../buffer.h"

struct OggVorbis_File;
struct vorbis_info;
extern "C" int ov_clear(OggVorbis_File *);

namespace audio
{
  namespace ogg
  {
    namespace detail
    {
      static bool little_endian();

      class file
      {
        public:
          using ogg_file_t = OggVorbis_File;

          file() = delete;
          file(std::string const &path);
          file(file const&) = delete;
          file(file &&) = delete;

          file& operator =(file const&) = delete;
          file& operator =(file &&) = delete;

          std::vector<char> read_all();
          std::vector<char> read_bytes(size_t const max_bytes);

          ALuint get_frequency() const
          { return m_freq; }
          format get_format() const
          { return m_format; }

        private:
          void open(std::string const &path);

          ALuint m_freq{};
          format m_format{ format::mono16 };
          vorbis_info *m_info{};
          std::unique_ptr<ogg_file_t, decltype(&ov_clear)> m_ogg;
      };
    }
  }
}
