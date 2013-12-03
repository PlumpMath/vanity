#include "application.h"

application::~application()
{
  /* Remove ourself as a window listener. */
  Ogre::WindowEventUtilities::removeWindowEventListener(m_window, this);
  windowClosed(m_window);
}

bool application::configure()
{
  // Show the configuration dialog and initialise the system
  // You can skip this and use root.restoreConfig() to load configuration
  // settings if you were sure there are valid ones saved in ogre.cfg
  if(m_root->showConfigDialog())
  {
    // If returned true, user clicked OK so initialise
    // Here we choose to let the system create a default rendering window by passing 'true'
    m_window = m_root->initialise(true, "Window");

    return true;
  }
  else
  { return false; }
}

void application::choose_scene_manager()
{
  /* Get the SceneManager; in this case, a generic one. */
  m_scene_mgr = m_root->createSceneManager(Ogre::ST_GENERIC);
}

void application::create_camera()
{
  /* Create the camera. */
  m_camera = m_scene_mgr->createCamera("PlayerCam");

  /* Look back along -Z. */
  m_camera->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
  m_camera->lookAt(Ogre::Vector3(0.0f, 0.0f, -300.0f));

  m_camera->setNearClipDistance(2.0f);

  m_camera_mgr.reset(new OgreBites::SdkCameraMan(m_camera));

  /* Setup some basic lighting. */
  m_scene_mgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
}

void application::create_frame_listener()
{
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
  OIS::ParamList pl;
  size_t windowHnd{ 0 };
  std::ostringstream windowHndStr;

  m_window->getCustomAttribute("WINDOW", &windowHnd);
  windowHndStr << windowHnd;
  pl.insert({"WINDOW", windowHndStr.str()});

  m_input_mgr = OIS::InputManager::createInputSystem(pl);

  m_keyboard = static_cast<OIS::Keyboard*>(m_input_mgr->createInputObject(OIS::OISKeyboard, true));
  m_mouse = static_cast<OIS::Mouse*>(m_input_mgr->createInputObject(OIS::OISMouse, true));

  m_mouse->setEventCallback(this);
  m_keyboard->setEventCallback(this);

  /* Set initial mouse clipping size. */
  windowResized(m_window);

  /* Register as a window listener. */
  Ogre::WindowEventUtilities::addWindowEventListener(m_window, this);

  m_root->addFrameListener(this);
}

void application::destroy_scene()
{
}

void application::create_viewports()
{
  /* Create one viewport, filling the entire window. */
  Ogre::Viewport* vp = m_window->addViewport(m_camera);
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
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

  Ogre::String secName, typeName, archName;
  while(seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for(i = settings->begin(); i != settings->end(); ++i)
    {
      typeName = i->first;
      archName = i->second;
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
  //Ogre::LogManager * const lm(new Ogre::LogManager());
  //lm->createLog("rpg_log", true, false, false);

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

  return true;
}

bool application::keyPressed(OIS::KeyEvent const &arg)
{
  if(arg.key == OIS::KC_F5) 
  { Ogre::TextureManager::getSingleton().reloadAll(); }
  else if (arg.key == OIS::KC_SYSRQ)
  { m_window->writeContentsToTimestampedFile("screenshot", ".png"); }
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

bool application::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
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
      m_input_mgr->destroyInputObject(m_keyboard);

      OIS::InputManager::destroyInputSystem(m_input_mgr);
      m_input_mgr = nullptr;
    }
  }
}
