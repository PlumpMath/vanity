/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: application.cpp
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    Undoubtedly copied from sample code.
*/

#include "application.h"
#include "log/logger.h"

application::~application()
{
  /* Remove ourself as a window listener. */
  Ogre::WindowEventUtilities::removeWindowEventListener(m_window, this);
  windowClosed(m_window);
}

bool application::configure()
{
  if(m_root->showConfigDialog())
  {
    m_window = m_root->initialise(true, "Window");

    return true;
  }
  else
  { return false; }
}

void application::choose_scene_manager()
{
  m_scene_mgr = m_root->createSceneManager(Ogre::ST_GENERIC);
}

void application::create_camera()
{
  m_camera = m_scene_mgr->createCamera("PlayerCam");

  /* Look back along -Z. */
  m_camera->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
  m_camera->lookAt(Ogre::Vector3(0.0f, 0.0f, -300.0f));

  m_camera->setNearClipDistance(2.0f);

  m_camera_mgr.reset(new OgreBites::SdkCameraMan(m_camera));

  m_scene_mgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
}

void application::create_frame_listener()
{
  OIS::ParamList pl;
  size_t windowHnd{};
  std::ostringstream windowHndStr;

  m_window->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert({"WINDOW", windowHndStr.str()});

  /* Show the cursor and don't capture everything. */
#if defined OIS_WIN32_PLATFORM
  pl.insert({"w32_mouse", "DISCL_FOREGROUND"});
  pl.insert({"w32_mouse", "DISCL_NONEXCLUSIVE"});
  pl.insert({"w32_keyboard", "DISCL_FOREGROUND"});
  pl.insert({"w32_keyboard", "DISCL_NONEXCLUSIVE"});
#elif defined OIS_LINUX_PLATFORM
  pl.insert({"x11_mouse_grab", "false"});
  pl.insert({"x11_mouse_hide", "false"});
  pl.insert({"x11_keyboard_grab", "false"});
  pl.insert({"XAutoRepeatOn", "true"});
#endif

  m_input_mgr = OIS::InputManager::createInputSystem(pl);

  m_keyboard = static_cast<OIS::Keyboard*>(m_input_mgr->createInputObject(OIS::OISKeyboard, true));
  m_mouse = static_cast<OIS::Mouse*>(m_input_mgr->createInputObject(OIS::OISMouse, true));

  m_mouse->setEventCallback(this);
  m_keyboard->setEventCallback(this);

  /* Set initial mouse clipping size. */
  windowResized(m_window);

  /* Register as a window and frame listener. */
  Ogre::WindowEventUtilities::addWindowEventListener(m_window, this);
  m_root->addFrameListener(this);
}

void application::destroy_scene()
{
}

void application::create_viewports()
{
  /* Create one viewport, filling the entire window. */
  Ogre::Viewport * const vp{ m_window->addViewport(m_camera) };
  vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

  /* Alter the camera aspect ratio to match the viewport. */
  m_camera->setAspectRatio(
      Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void application::setup_resources()
{
  /* Load resource paths from config file. */
  Ogre::ConfigFile cf;
  cf.load(m_resources_cfg);

  /* Go through all sections & settings in the file. */
  Ogre::ConfigFile::SectionIterator seci{ cf.getSectionIterator() };

  Ogre::String secName, typeName, archName;
  while(seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap * const settings{ seci.getNext() };
    for(auto const &i : *settings)
    {
      typeName = i.first;
      archName = i.second;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          archName, typeName, secName);
    }
  }
}

void application::create_resource_listener()
{
}

void application::load_resources()
{
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void application::go()
{
#ifdef _DEBUG
  m_resources_cfg = "resources_d.cfg";
  m_plugins_cfg = "plugins_d.cfg";
#else
  m_resources_cfg = "resources.cfg";
  m_plugins_cfg = "plugins.cfg";
#endif

  if(!setup())
  { return; }

  /* Enter game loop. */
  m_root->startRendering();

  /* Cleanup. */
  destroy_scene();
}

bool application::setup()
{
  Ogre::LogManager * const lm{ new Ogre::LogManager() };
  lm->createLog("rpg_log", true, false, false);

  m_root.reset(new Ogre::Root(m_plugins_cfg));

  setup_resources();

  auto const carryOn(configure());
  if(!carryOn)
  { return false; }

  choose_scene_manager();
  create_camera();
  create_viewports();

  /* Set default mipmap level (NB some APIs ignore this). */
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  /* Create any resource listeners (for loading screens). */
  create_resource_listener();
  load_resources();
  create_scene();
  create_frame_listener();

  return true;
}

bool application::frameRenderingQueued(Ogre::FrameEvent const &evt)
{
  if(m_window->isClosed())
  { return false; }

  if(m_shutdown)
  { return false; }

  /* Need to capture/update each device. */
  m_keyboard->capture();
  m_mouse->capture();

  /* If dialog isn't up, then update the camera. */
  m_camera_mgr->frameRenderingQueued(evt);

  return frame_rendering_queued(evt);
}

bool application::keyPressed(OIS::KeyEvent const &arg)
{
  if(arg.key == OIS::KC_F5) 
  {
    Ogre::TextureManager::getSingleton().reloadAll();
    log_info("All textures reloaded");
  }
  else if (arg.key == OIS::KC_SYSRQ)
  {
    m_window->writeContentsToTimestampedFile("screenshot", ".png");
    log_info("Screenshot saved");
  }
  else if (arg.key == OIS::KC_ESCAPE)
  { m_shutdown = true; }

  return key_pressed(arg);
}

bool application::keyReleased(OIS::KeyEvent const &arg)
{ return key_released(arg); }

bool application::mouseMoved(OIS::MouseEvent const &arg)
{ return mouse_moved(arg); }

bool application::mousePressed(OIS::MouseEvent const &arg, OIS::MouseButtonID id)
{ return mouse_pressed(arg, id); }

bool application::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{ return mouse_released(arg, id); }

void application::windowResized(Ogre::RenderWindow *rw)
{
  unsigned int width{}, height{}, depth{};
  int left{}, top{};
  rw->getMetrics(width, height, depth, left, top);

  OIS::MouseState const &ms{ m_mouse->getMouseState() };
  ms.width = width;
  ms.height = height;
}

/* Unattach OIS before window shutdown (very important under Linux). */
void application::windowClosed(Ogre::RenderWindow *rw)
{
  /* Only close for window that created OIS. */
  if(rw == m_window)
  {
    if(m_input_mgr)
    {
      m_input_mgr->destroyInputObject(m_mouse);
      m_mouse = nullptr;
      m_input_mgr->destroyInputObject(m_keyboard);
      m_keyboard = nullptr;

      OIS::InputManager::destroyInputSystem(m_input_mgr);
      m_input_mgr = nullptr;
    }
  }
}
