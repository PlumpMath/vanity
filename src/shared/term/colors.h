/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: term/colors.h
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#pragma once

namespace term
{
  namespace colors
  {
    /* Term. */
    char const * const off{ "\033[0m" };
    char const * const black{ "\033[0;30m" };
    char const * const red{ "\033[0;31m" };
    char const * const green{ "\033[0;32m" };
    char const * const yellow{ "\033[0;33m" };
    char const * const blue{ "\033[0;34m" };
    char const * const purple{ "\033[0;35m" };
    char const * const cyan{ "\033[0;36m" };
    char const * const white{ "\033[0;37m" };
  }
}
