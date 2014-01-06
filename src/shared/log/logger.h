/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: log/logger.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

#include <map>
#include <cstdint>
#include <thread>
#include <mutex>
#include <sstream>

#include "format.h"
#include "macros.h"

namespace logging
{
  class logger
  {
    public:
      using push_t = uint8_t;

      enum class verbosity
      { none, error, info, debug };

      struct scoped_push
      {
        scoped_push()
        { log_push(); }
        ~scoped_push()
        { log_pop(); }
      };

      static logger& get();

      void debug(std::string const &module, std::string const &msg);
      void info(std::string const &module, std::string const &msg);
      void error(std::string const &module, std::string const &msg);

      void push();
      void pop();

    private:
      void render(verbosity const verb, std::string const &module, std::string const &msg);
      
      verbosity m_verbosity{ verbosity::debug };
      uint8_t m_push_level{};
      std::mutex m_render_lock;

      /* Thread naming. */
      std::map<std::thread::id, uint64_t> m_thread_ids;
      uint64_t m_last_id{};
      std::mutex m_thread_ids_lock;
  };
}
