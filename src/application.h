#pragma once

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

class application
    : public Ogre::FrameListener
    , public Ogre::WindowEventListener
    , public OIS::KeyListener
    , public OIS::MouseListener
    , private OgreBites::SdkTrayListener
{
public:
    application() = default;
    virtual ~application();

    virtual void go();

protected:
    virtual bool setup();
    virtual bool configure();
    virtual void choose_scene_manager();
    virtual void create_camera();
    virtual void create_frame_listener();
    virtual void create_scene() = 0;
    virtual void destroy_scene();
    virtual void create_viewports();
    virtual void setup_resources();
    virtual void create_resource_listener();
    virtual void load_resources();

    // Ogre::FrameListener
    virtual bool frameRenderingQueued(Ogre::FrameEvent const &evt) override;

    // OIS::KeyListener
    bool keyPressed(OIS::KeyEvent const &arg) override;
    bool keyReleased(OIS::KeyEvent const &arg) override;
    // OIS::MouseListener
    bool mouseMoved(OIS::MouseEvent const &arg) override;
    bool mousePressed(OIS::MouseEvent const &arg, OIS::MouseButtonID id) override;
    bool mouseReleased(OIS::MouseEvent const &arg, OIS::MouseButtonID id) override;

    // Ogre::WindowEventListener
    //Adjust mouse clipping area
    void windowResized(Ogre::RenderWindow *rw) override;
    //Unattach OIS before window shutdown (very important under Linux)
    void windowClosed(Ogre::RenderWindow *rw) override;

    Ogre::Root *m_root{ nullptr };
    Ogre::Camera *m_camera{ nullptr };
    Ogre::SceneManager *m_scene_mgr{ nullptr };
    Ogre::RenderWindow *m_window{ nullptr };
    Ogre::String m_resources_cfg;
    Ogre::String m_plugins_cfg;

    // OgreBites
    OgreBites::SdkTrayManager *m_tray_mgr{ nullptr };
    OgreBites::SdkCameraMan *m_camera_mgr{ nullptr };
    OgreBites::ParamsPanel *m_details_panel{ nullptr };
    bool m_cursorWasVisible{ false };
    bool m_shutDown{ false };

    //OIS Input devices
    OIS::InputManager *m_input_mgr{ nullptr };
    OIS::Mouse *m_mouse{ nullptr };
    OIS::Keyboard *m_keyboard{ nullptr };
};