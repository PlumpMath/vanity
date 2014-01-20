/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/dispatch/reciever.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Wraps Awesomium's V8 interface with type-safety
    to allow for V8 agnostic-callbacks that recieve
    arguments like any other C++ function. Callbacks
    with any number of arguments are supported; return
    types are limited to void and the internal variant
    since that type needs to go back to V8.
*/

#pragma once

#include <Awesomium/WebCore.h>
#include <Awesomium/STLHelpers.h>

#include <stdexcept>
#include <vector>
#include <memory> 
#include <map>

#include "variant_traits.h"

namespace ui
{
  namespace dispatch
  {
    using variant_t = Awesomium::JSValue;

    /* Generates an integer sequenced type. T<4>::type becomes T<0, 1, 2, 3, 4> */
    /* XXX: C++14 gives std::integer_sequence for this. */
    template <size_t... Ns> struct int_seq{ };
    template <size_t... Ns> struct make_int_seq;
    template <size_t N, size_t... Ns> struct make_int_seq<N, Ns...>
                                              : make_int_seq<N - 1, N, Ns...>{ };
    template <size_t... Ns> struct make_int_seq<0, Ns...>
    { using type = int_seq<0, Ns...>; };

    /* Used to determine if the argument count is non-zero. */
    template <size_t N, size_t M>
    struct is_same : std::false_type{ };
    template <size_t N>
    struct is_same<N, N> : std::true_type{ };

    template <typename Ret>
    struct callback 
    { virtual Ret operator ()(Awesomium::JSArray const&) const = 0; };

    template <typename Ret, typename... Args>
    class typed_callback final : private callback<Ret>
    {
      typed_callback(std::function<Ret (Args...)> const &fun)
        : m_fun(fun)
      { }

      Ret operator ()(Awesomium::JSArray const &vec) const override
      { return seq_forward(vec, is_same<sizeof...(Args), 0>()); }

      /* Nullary functions don't care about the argument list. */
      Ret seq_forward(Awesomium::JSArray const &, std::true_type const) const
      { return m_fun(); }

      /* Generate an integer sequence for non-nullary functions (avoids underflow). */
      Ret seq_forward(Awesomium::JSArray const &vec, std::false_type const) const
      { return forward(vec, typename make_int_seq<sizeof...(Args) - 1>::type{ }); }

      template <size_t... Ns>
      Ret forward(Awesomium::JSArray const &args, int_seq<Ns...> const) const
      {
        if(args.size() < sizeof...(Args))
        { throw std::invalid_argument(std::string{"Invalid number of arguments "} +
                                      "(expected " + std::to_string(sizeof...(Args)) +
                                      " recieved " + std::to_string(args.size()) + ")"); }

        return m_fun(variant<Args>::get(args[Ns]) ...);
      }

      template <typename Ret_, typename... Args_>
      friend std::unique_ptr<callback<Ret_>> make_callback(std::function<Ret_ (Args_...)> const&);

      std::function<Ret (Args...)> const m_fun;
    };

    template <typename Ret, typename... Args>
    std::unique_ptr<callback<Ret>> make_callback(std::function<Ret (Args...)> const &fun)
    { return std::unique_ptr<callback<Ret>>(new typed_callback<Ret, Args...>(fun)); }

    class reciever : public Awesomium::JSMethodHandler
    {
      public:
        using key_t = std::pair<unsigned int const, std::string const>;

        template <typename Ret, typename... Args>
        void subscribe(Awesomium::JSObject &obj, std::string const &name,
                       std::function<Ret (Args...)> const &fun)
        {
          if(obj.type() == Awesomium::kJSObjectType_Local)
          { throw std::invalid_argument("Cannot subscribe function: JSObject is local"); }

          add_callback(obj, name, fun);
        }

        /* JSMethodHandler interface. */
        void OnMethodCall(Awesomium::WebView *caller,
                          unsigned int remote_object_id, 
                          const Awesomium::WebString &method_name,
                          const Awesomium::JSArray &args) override
        {
          auto const it(callbacks.find(
                        { remote_object_id, Awesomium::ToString(method_name) }));
          if(it != callbacks.end())
          {
            /* Invoke each callback. */
            for(auto const &cb : it->second)
            { (*cb)(args); }
          }
        }
        Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView *caller,
                                                       unsigned int remote_object_id,
                                                       const Awesomium::WebString &method_name,
                                                       const Awesomium::JSArray &args) override
        {
          auto const it(callbacks_with_return.find(
                        { remote_object_id, Awesomium::ToString(method_name) }));
          if(it != callbacks_with_return.end())
          { return (*it->second)(args); }
          
          return Awesomium::JSValue::Undefined();
        }

      private:
        /* Generic way of saving a callback (either with return type or not). */
        template <typename... Args>
        void add_callback(Awesomium::JSObject &obj, std::string const &name,
                          std::function<void (Args...)> const &fun)
        {
          obj.SetCustomMethod(Awesomium::WSLit(name.c_str()), false);
          callbacks[{ obj.remote_id(), name }].push_back(make_callback(fun));
        }

        template <typename Ret, typename... Args>
        void add_callback(Awesomium::JSObject &obj, std::string const &name,
                          std::function<Ret (Args...)> const &fun)
        {
          obj.SetCustomMethod(Awesomium::WSLit(name.c_str()), true);
          callbacks_with_return.emplace(key_t{ obj.remote_id(), name },
                                        std::move(make_callback(fun)));
        }

        std::map<key_t, std::vector<std::unique_ptr<callback<void>>>> callbacks;
        std::map<key_t, std::unique_ptr<callback<variant_t>> const> callbacks_with_return;
    };
  }
}
