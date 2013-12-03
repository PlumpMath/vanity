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

#include "borrowed_ptr.h"

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

    /*** Ogre::FrameListener. ***/
    bool frameRenderingQueued(Ogre::FrameEvent const &evt) override final;
    virtual bool frame_rendering_queued(Ogre::FrameEvent const &evt) { return true; }

    /*** OIS::KeyListener. ***/
    bool keyPressed(OIS::KeyEvent const &arg) override final;
    bool keyReleased(OIS::KeyEvent const &arg) override final;
    virtual bool key_pressed(OIS::KeyEvent const &arg) = 0;
    virtual bool key_released(OIS::KeyEvent const &arg) = 0;

    /*** OIS::MouseListener. ***/
    bool mouseMoved(OIS::MouseEvent const &arg) override final;
    bool mousePressed(OIS::MouseEvent const &arg, OIS::MouseButtonID id) override final;
    bool mouseReleased(OIS::MouseEvent const &arg, OIS::MouseButtonID id) override final;
    virtual bool mouse_moved(OIS::MouseEvent const &arg) = 0;
    virtual bool mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id) = 0;
    virtual bool mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id) = 0;

    /*** Ogre::WindowEventListener. ***/
    /* Adjust mouse clipping area. */
    void windowResized(Ogre::RenderWindow *rw) override final;
    /* Unattach OIS before window shutdown (very important under Linux). */
    void windowClosed(Ogre::RenderWindow *rw) override final;

    /* Scene */
    std::unique_ptr<Ogre::Root> m_root{ nullptr };
    borrowed_ptr<Ogre::Camera> m_camera{ nullptr };
    borrowed_ptr<Ogre::SceneManager> m_scene_mgr{ nullptr };
    borrowed_ptr<Ogre::RenderWindow> m_window{ nullptr };
    Ogre::String m_resources_cfg;
    Ogre::String m_plugins_cfg;
    bool m_shutdown{ false };

    /* OgreBites. */
    std::unique_ptr<OgreBites::SdkCameraMan> m_camera_mgr{ nullptr };

    /* OIS Input devices. */
    borrowed_ptr<OIS::InputManager> m_input_mgr{ nullptr };
    borrowed_ptr<OIS::Mouse> m_mouse{ nullptr };
    borrowed_ptr<OIS::Keyboard> m_keyboard{ nullptr };
};
