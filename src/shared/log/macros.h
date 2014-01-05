/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: log/macros.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

/* Message logging. */
#define log_debug(...) \
{ \
  std::stringstream ss; \
  logging::variadic_format(ss).print(__VA_ARGS__); \
  logging::logger::get().debug(__FILE__, ss.str()); \
}
#define log_info(...) \
{ \
  std::stringstream ss; \
  logging::variadic_format(ss).print(__VA_ARGS__); \
  logging::logger::get().info(__FILE__, ss.str()); \
}
#define log_error(...) \
{ \
  std::stringstream ss; \
  logging::variadic_format(ss).print(__VA_ARGS__); \
  logging::logger::get().error(__FILE__, ss.str()); \
}

/* Indentation. */
#define log_push() \
  logging::logger::get().push();
#define log_pop() \
  logging::logger::get().pop();
#define log_scoped_push() \
  auto const scoped_push##__LINE__((logging::logger::scoped_push()));

/* Assertions. */
#define log_fail(message) \
  log_error(message); \
  assert(false);
#define log_assert(condition, message) \
  if(!(condition)) \
  { log_fail(message); } 
