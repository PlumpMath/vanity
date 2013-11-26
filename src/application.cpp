#include "application.h"

application::~application()
{
  delete m_tray_mgr;
  delete m_camera_mgr;

  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(m_window, this);
  windowClosed(m_window);
  delete m_root;
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
  // Get the SceneManager, in this case a generic one
  m_scene_mgr = m_root->createSceneManager(Ogre::ST_GENERIC);
}

void application::create_camera()
{
  // Create the camera
  m_camera = m_scene_mgr->createCamera("PlayerCam");

  m_camera->setPosition(Ogre::Vector3(0.0f, 0.0f, 0.0f));
  // Look back along -Z
  m_camera->lookAt(Ogre::Vector3(0.0f, 0.0f, -300.0f));
  m_camera->setNearClipDistance(5.0f);

  m_camera_mgr = new OgreBites::SdkCameraMan(m_camera);   // create a default camera controller

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

  m_input_mgr = OIS::InputManager::createInputSystem( pl );

  m_keyboard = static_cast<OIS::Keyboard*>(m_input_mgr->createInputObject( OIS::OISKeyboard, true ));
  m_mouse = static_cast<OIS::Mouse*>(m_input_mgr->createInputObject( OIS::OISMouse, true ));

  m_mouse->setEventCallback(this);
  m_keyboard->setEventCallback(this);

  //Set initial mouse clipping size
  windowResized(m_window);

  //Register as a Window listener
  Ogre::WindowEventUtilities::addWindowEventListener(m_window, this);

  m_tray_mgr = new OgreBites::SdkTrayManager("InterfaceName", m_window, m_mouse, this);
  m_tray_mgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
  //m_trayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
  m_tray_mgr->hideCursor();

  // create a params panel for displaying sample details
  Ogre::StringVector items;
  items.push_back("cam.pX");
  items.push_back("cam.pY");
  items.push_back("cam.pZ");
  items.push_back("");
  items.push_back("cam.oW");
  items.push_back("cam.oX");
  items.push_back("cam.oY");
  items.push_back("cam.oZ");
  items.push_back("");
  items.push_back("Filtering");
  items.push_back("Poly Mode");

  m_details_panel = m_tray_mgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
  m_details_panel->setParamValue(9, "Bilinear");
  m_details_panel->setParamValue(10, "Solid");
  m_details_panel->hide();

  m_root->addFrameListener(this);
}

void application::destroy_scene()
{
}

void application::create_viewports()
{
  // Create one viewport, entire window
  Ogre::Viewport* vp = m_window->addViewport(m_camera);
  vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

  // Alter the camera aspect ratio to match the viewport
  m_camera->setAspectRatio(
      Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}

void application::setup_resources()
{
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load(m_resources_cfg);

  // Go through all sections & settings in the file
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

  Ogre::String secName, typeName, archName;
  while (seci.hasMoreElements())
  {
    secName = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;
    for (i = settings->begin(); i != settings->end(); ++i)
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
    return;

  m_root->startRendering();

  // clean up
  destroy_scene();
}

bool application::setup()
{
  //Ogre::LogManager * const lm(new Ogre::LogManager());
  //lm->createLog("rpg_log", true, false, false);

  m_root = new Ogre::Root(m_plugins_cfg);

  setup_resources();

  auto const carryOn(configure());
  if (!carryOn) return false;

  choose_scene_manager();
  create_camera();
  create_viewports();

  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

  // Create any resource listeners (for loading screens)
  create_resource_listener();
  // Load resources
  load_resources();

  // Create the scene
  create_scene();

  create_frame_listener();

  return true;
}

bool application::frameRenderingQueued(Ogre::FrameEvent const &evt)
{
  if(m_window->isClosed())
    return false;

  if(m_shutDown)
    return false;

  //Need to capture/update each device
  m_keyboard->capture();
  m_mouse->capture();

  m_tray_mgr->frameRenderingQueued(evt);

  if (!m_tray_mgr->isDialogVisible())
  {
    m_camera_mgr->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
    if (m_details_panel->isVisible())   // if details panel is visible, then update its contents
    {
      m_details_panel->setParamValue(0, Ogre::StringConverter::toString(m_camera->getDerivedPosition().x));
      m_details_panel->setParamValue(1, Ogre::StringConverter::toString(m_camera->getDerivedPosition().y));
      m_details_panel->setParamValue(2, Ogre::StringConverter::toString(m_camera->getDerivedPosition().z));
      m_details_panel->setParamValue(4, Ogre::StringConverter::toString(m_camera->getDerivedOrientation().w));
      m_details_panel->setParamValue(5, Ogre::StringConverter::toString(m_camera->getDerivedOrientation().x));
      m_details_panel->setParamValue(6, Ogre::StringConverter::toString(m_camera->getDerivedOrientation().y));
      m_details_panel->setParamValue(7, Ogre::StringConverter::toString(m_camera->getDerivedOrientation().z));
    }
  }

  return true;
}

bool application::keyPressed( const OIS::KeyEvent &arg )
{
  if (m_tray_mgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

  if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
  {
    m_tray_mgr->toggleAdvancedFrameStats();
  }
  else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
  {
    if (m_details_panel->getTrayLocation() == OgreBites::TL_NONE)
    {
      m_tray_mgr->moveWidgetToTray(m_details_panel, OgreBites::TL_TOPRIGHT, 0);
      m_details_panel->show();
    }
    else
    {
      m_tray_mgr->removeWidgetFromTray(m_details_panel);
      m_details_panel->hide();
    }
  }
  else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
  {
    Ogre::String newVal;
    Ogre::TextureFilterOptions tfo;
    unsigned int aniso;

    switch (m_details_panel->getParamValue(9).asUTF8()[0])
    {
      case 'B':
        newVal = "Trilinear";
        tfo = Ogre::TFO_TRILINEAR;
        aniso = 1;
        break;
      case 'T':
        newVal = "Anisotropic";
        tfo = Ogre::TFO_ANISOTROPIC;
        aniso = 8;
        break;
      case 'A':
        newVal = "None";
        tfo = Ogre::TFO_NONE;
        aniso = 1;
        break;
      default:
        newVal = "Bilinear";
        tfo = Ogre::TFO_BILINEAR;
        aniso = 1;
    }

    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
    Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
    m_details_panel->setParamValue(9, newVal);
  }
  else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
  {
    Ogre::String newVal;
    Ogre::PolygonMode pm;

    switch (m_camera->getPolygonMode())
    {
      case Ogre::PM_SOLID:
        newVal = "Wireframe";
        pm = Ogre::PM_WIREFRAME;
        break;
      case Ogre::PM_WIREFRAME:
        newVal = "Points";
        pm = Ogre::PM_SOLID;
        break;
      default:
        newVal = "Solid";
        pm = Ogre::PM_SOLID;
    }

    m_camera->setPolygonMode(pm);
    m_details_panel->setParamValue(10, newVal);
  }
  else if(arg.key == OIS::KC_F5)   // refresh all textures
  { Ogre::TextureManager::getSingleton().reloadAll(); }
  else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
  { m_window->writeContentsToTimestampedFile("screenshot", ".png"); }
  else if (arg.key == OIS::KC_ESCAPE)
  { m_shutDown = true; }

  m_camera_mgr->injectKeyDown(arg);
  return true;
}

bool application::keyReleased( const OIS::KeyEvent &arg )
{
  m_camera_mgr->injectKeyUp(arg);

  return true;
}

bool application::mouseMoved( const OIS::MouseEvent &arg )
{
  if(m_tray_mgr->injectMouseMove(arg))
  { return true; }

  m_camera_mgr->injectMouseMove(arg);

  return true;
}

bool application::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  if(m_tray_mgr->injectMouseDown(arg, id))
  { return true; }

  m_camera_mgr->injectMouseDown(arg, id);

  return true;
}

bool application::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
  if(m_tray_mgr->injectMouseUp(arg, id))
  { return true; }

  m_camera_mgr->injectMouseUp(arg, id);

  return true;
}

//Adjust mouse clipping area
void application::windowResized(Ogre::RenderWindow* rw)
{
  unsigned int width{ 0 }, height{ 0 }, depth{ 0 };
  int left{ 0 }, top{ 0 };
  rw->getMetrics(width, height, depth, left, top);

  OIS::MouseState const &ms{ m_mouse->getMouseState() };
  ms.width = width;
  ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void application::windowClosed(Ogre::RenderWindow* rw)
{
  //Only close for window that created OIS
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
