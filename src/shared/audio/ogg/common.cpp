/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: audio/ogg/common.cpp
  Author: Jesse 'Jeaye' Wilkerson
*/

#include "common.h"

#include <vorbis/vorbisfile.h>

#include <stdexcept>
#include <limits>

namespace audio
{
  namespace ogg
  {
    namespace detail
    {
      static bool little_endian()
      {
        int16_t const num{ 0x1 };
        char const * const num_ptr{ reinterpret_cast<char const*>(&num) };
        return (num_ptr[0] == 1);
      }

      file::file(std::string const &path)
        : m_ogg(new ogg_file_t{}, &ov_clear)
      { open(path); }

      std::vector<char> file::read_all()
      { return read_bytes(std::numeric_limits<size_t>::max()); }

      std::vector<char> file::read_bytes(size_t const max_bytes)
      {
        size_t const chunk{ std::min<size_t>(max_bytes, 4096) };
        std::vector<char> out(chunk);

        /* Output param from OV; unused. */
        int32_t bit_stream{};

        /* We may not read a whole chunk, so keep track of how
         * far behind we are. */
        size_t accumulated_diff{};
        size_t total_read{};

        for(size_t i{}; ; ++i)
        {
          /* Determine how much to read next. */
          size_t const next_chunk{ std::min(max_bytes - total_read, chunk) };
          if(next_chunk == 0)
          {
            out.resize(total_read);
            break;
          }
          if(total_read > max_bytes)
          { throw std::runtime_error("Read too much ogg"); }

          auto const read( ::ov_read(m_ogg.get(),
                out.data() + (chunk * i) - accumulated_diff, next_chunk,
                !little_endian(), 2, 1, &bit_stream));

          if(read < 0)
          { throw std::runtime_error("Error reading from ogg file"); }
          else if(!read)
          {
            /* Done reading, remove the last unused chunk. */
            out.resize(out.size() - chunk);
            break;
          }

          /* We may not've read a full chunk. */
          size_t const diff{ chunk - read };
          accumulated_diff += diff;
          total_read += read;

          /* Size up for another chunk. */
          out.resize(out.size() + (chunk - diff));
        } 

        return out;
      }

      void file::open(std::string const &path)
      {
        if(!path.size())
        { throw std::invalid_argument("Invalid file path"); }

        /* No need to fclose after giving OV the file. */
        FILE * const fp{ std::fopen(path.c_str(), "rb") };
        if(!fp)
        { throw std::runtime_error("Unable to open file " + path); }

        if(::ov_open(fp, m_ogg.get(), nullptr, 0) < 0) /* TODO: Maybe not Win-compatible? */
        {
          std::fclose(fp);
          throw std::runtime_error("Failed to ogg open " + path);
        }

        m_info = ::ov_info(m_ogg.get(), -1);
        if(!m_info)
        { throw std::runtime_error("Failed to acquire ogg info for " + path); }

        m_format = (m_info->channels == 1 ? format::mono16 : format::stereo16);
        m_freq = m_info->rate;
      }
    }
  }
}
