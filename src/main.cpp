/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: main.cpp
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#include "game.h"

#include <cef_app.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  INT WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
#else
  int main(int argc, char **argv)
#endif
  {
    CefMainArgs args(argc, argv);
    int const code{ CefExecuteProcess(args, nullptr, nullptr) };
    //int const code{ CefExecuteProcess(args, nullptr) };
    if(code >= 0) /* A child proccess has ended, so exit. */
    { return code; }
    else if(code == -1)
    {
      /* This is the parent process. */
    }

    CefSettings settings;
    bool const result{ CefInitialize(args, settings, nullptr, nullptr) };
    //bool const result{ CefInitialize(args, settings, nullptr) };
    if(!result)
    { return -1; }

    game app;
    try
    { app.go(); }
    catch(Ogre::Exception &e)
    {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
      MessageBox(nullptr, e.getFullDescription().c_str(),
                 "An exception has occured!",
                 MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
      std::cerr << "An exception has occured: "
                << e.getFullDescription() << std::endl;
#endif
    }
    catch(std::exception &e)
    { std::cerr << "An exception was thrown: " << e.what() << std::endl; }
    catch(...)
    { std::cerr << "An unkown exception was thrown" << std::endl; }

    CefShutdown();
  }

#ifdef __cplusplus
}
#endif

