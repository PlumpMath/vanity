/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/dispatch/sender.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps Awesomium's V8 interface with type-safety
    to allow for stringly V8-agnostic function calls.
*/

#pragma once

#include <Awesomium/STLHelpers.h>

#include <sstream>
#include <string>

#include "util/borrowed_ptr.h"

namespace ui
{
  namespace dispatch
  {
    namespace sender
    {
      /* All params need to be stringitized before handing to JS. */
      template <typename T>
      void write_arg(std::stringstream &ss, T const &t)
      { ss << t; }

      /* Strings and chars must be quoted. */
      void write_arg(std::stringstream &ss, std::string const &t)
      { ss << "\"" << t << "\""; }
      void write_arg(std::stringstream &ss, char const * const &t)
      { ss << "\"" << t << "\""; }
      void write_arg(std::stringstream &ss, char const &t)
      { ss << "'" << t << "'"; }

      void write_args(std::stringstream &ss)
      { }
      template <typename T>
      void write_args(std::stringstream &ss, T const &t)
      { write_arg(ss, t); } /* The last param gets no comma after it. */
      template <typename T, typename... Args>
      void write_args(std::stringstream &ss, T const &t, Args... args)
      {
        write_arg(ss, t);
        ss << ",";
        write_args(ss, args...);
      }
      template <typename... Args>
      std::string stringitize(std::string const &func, Args... args)
      {
        std::stringstream ss;
        ss << std::boolalpha;
        ss << func << "(";
        write_args(ss, args...);
        ss << ");";
        return ss.str();
      }

      /* Post a JS call *asynchronously* and disregard any return value. */
      template <typename WebView, typename... Args>
      void post(borrowed_ptr<WebView> const wv, std::string const &func, Args... args)
      {
        std::string const str(stringitize(func, args...));
        wv->ExecuteJavascript(Awesomium::ToWebString(str), Awesomium::WebString{});
      }

      /* Post a JS call *synchronously* and forward on the return value. */
      template <typename Ret, typename WebView, typename... Args>
      Ret post_sync(borrowed_ptr<WebView> const wv, std::string const &func, Args... args)
      {
        std::string const str(stringitize(func, args...));
        auto const res(wv->ExecuteJavascriptWithResult(Awesomium::ToWebString(str),
                                                       Awesomium::WebString{}));
        /* Convert the JS type to a C++ type. */
        return variant<Ret>::get(res);
      }
    }
  }
}
