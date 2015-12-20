/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#if not defined (__unix__) && not defined(__unix) && not defined (unix) && ( (not defined (__APPLE__) || not defined (__MACH__)) )
  #error THIS SOFTWARE IS ONLY FOR UNIX-LIKE SYSTEMS!
#endif

#pragma once

#include <memory>
#include <iostream>
#include <dlfcn.h>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <cassert>

namespace dynamicLinker {

  class dynamicLinkerException : public std::runtime_error {
  public:
    dynamicLinkerException ( const std::string& what_arg ) :
      std::runtime_error( what_arg ) {}
    dynamicLinkerException ( const char* what_arg ) :
      std::runtime_error( what_arg ) {}
  };

  class openException : public dynamicLinkerException {
  public:
    openException(std::string s) : dynamicLinkerException("ERROR: Cannot open dynamic library: " + s) {}
  };

  class symbolException : public dynamicLinkerException {
  public:
    symbolException(std::string s) : dynamicLinkerException("ERROR: Cannot find symbol: " + s) {}
  };

  class closedException : public dynamicLinkerException {
  public:
    closedException() : dynamicLinkerException("ERROR: Library was not opened!") {}
  };

  class dynamicLinker : public std::enable_shared_from_this<dynamicLinker> {
  private:

    class _void {
    private:
      void * myself = nullptr;
    public:
      _void( void * );
      ~_void();
      void * ptr() const;
      void null();
    };


    template<typename R, typename A> class dlSymbol {
    private:
      std::function< R(A) > sym = nullptr;
      std::shared_ptr<dynamicLinker> parent = nullptr;
    public:
      dlSymbol( std::shared_ptr<dynamicLinker> p, std::function< R(A) > s )
        : sym(s), parent(p) {}
      R operator()(A arg) {
        return sym(arg);
      }
      std::function< R(A) > raw() {
        return sym;
      }
    };


    std::string libPath = "";
    std::unique_ptr<_void> lib = nullptr;
    dynamicLinker();
    dynamicLinker( std::string );
  public:
    static std::shared_ptr<dynamicLinker> make_new( std::string );
    ~dynamicLinker();
    bool open();
    template<typename R, typename A> dlSymbol<R,A> getFunction( std::string name ) {

      if( lib == nullptr )
        throw closedException();

      auto sym = dlSymbol<R,A>( shared_from_this(), std::function< R(A) >(reinterpret_cast<  R(*)(A)  >(  dlsym(lib->ptr(), name.c_str())  )) );

      if( sym.raw() == nullptr ) {
        char* err = dlerror();
        std::string s = (err == nullptr) ? "FATAL ERROR: No error!" : std::string(err);
        throw symbolException(s);
      }
      return sym;
    }
  };

};