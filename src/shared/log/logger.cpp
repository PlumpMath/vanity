/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: log/logger.cpp
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#include "logger.h"
#include "term/colors.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <chrono>
#include <iomanip>

namespace logging
{
  logger& logger::get()
  {
    static logger l;

    /* Cache this thread name. */
    std::lock_guard<std::mutex> const lock(l.m_thread_ids_lock);
    auto &id(l.m_thread_ids[std::this_thread::get_id()]);
    if(id == 0)
    { id = ++l.m_last_id; }

    return l;
  }

  void logger::debug(std::string const &module, std::string const &msg)
  {
    if(m_verbosity >= logger::verbosity::debug)
    { render(verbosity::debug, module, msg); }
  }
  void logger::info(std::string const &module, std::string const &msg)
  {
    if(m_verbosity >= logger::verbosity::info)
    { render(verbosity::info, module, msg); }
  }
  void logger::error(std::string const &module, std::string const &msg)
  {
    if(m_verbosity >= logger::verbosity::error)
    { render(verbosity::error, module, msg); }
  }

  void logger::push()
  {
    if(m_push_level == std::numeric_limits<logger::push_t>::max())
    { throw std::overflow_error("Reached maximum log push level"); }
    ++m_push_level;
  }
  void logger::pop()
  {
    if(m_push_level == 0)
    { throw std::underflow_error("Cannot decrease push level anymore"); }
    --m_push_level;
  }

  std::string strip_module(std::string const &module)
  {
    static std::string const src{ "src/" };
    std::string out;

    /* Remove src/ */
    auto const found_src(module.find(src));
    if(found_src != std::string::npos)
    { out = module.substr(found_src + src.size()); }
    else /* Out of source. */
    { return module; }

    /* Remove the extension. */
    auto const found_extension(out.find_last_of('.'));
    if(found_extension != std::string::npos)
    { out.erase(found_extension); }
    else
    { throw std::runtime_error("Logging source file does not have extension"); }

    return out;
  }

  void logger::render(logger::verbosity const verb, std::string const &full_module, std::string const &msg)
  {
    auto const module(strip_module(full_module));

    std::lock_guard<std::mutex> const render_lock(m_render_lock);
    std::lock_guard<std::mutex> const thread_ids_lock(m_thread_ids_lock);

    std::stringstream ss(msg);
    std::string line;
    while(std::getline(ss, line, '\n'))
    { render_line(verb, module, line); }
  }

  void logger::render_line(logger::verbosity const verb,
                           std::string const &module, std::string const &msg)
  {
    /* Module */
    std::cout << term::colors::white
      << "[" << module << "]";

    /* Thread */
    std::cout << term::colors::purple
      << "(" << std::setw(3) << std::setfill('0')
      << m_thread_ids[std::this_thread::get_id()] << ")";

    for(logger::push_t i{}; i < m_push_level; ++i)
    { std::cout << "  "; }

    switch(verb)
    {
      case logger::verbosity::debug:
        std::cout << term::colors::green << " debug => ";
        break;
      case logger::verbosity::info:
        std::cout << term::colors::yellow << " info => ";
        break;
      case logger::verbosity::error:
        std::cout << term::colors::red << " error => ";
        break;

      case logger::verbosity::none:
      default:
        { throw std::invalid_argument("Unknown verbosity level"); }
    }

    std::cout << term::colors::off << msg << std::endl;
  }
}
