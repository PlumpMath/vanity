/*
  Copyright 2013 Jesse 'Jeaye' Wilkerson
  See licensing in LICENSE file, or at:
    http://www.opensource.org/licenses/BSD-3-Clause

  File: ui/window.cpp
  Author: Jesse 'Jeaye' Wilkerson
  Description:
    TODO
*/

#include "window.h"
#include "input_dispatcher.h"

#include <cef_app.h>
#include <cef_client.h>
#include <cef_render_handler.h>

#include <OgreMeshManager.h>
#include <OgreManualObject.h>

namespace ui
{
  window::window(borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
                 float const w, float const h)
    : m_url(url)
  {
    load(sm, url, w, h);
    input_dispatcher::global().add_window(*this);
  }

  window::~window()
  { input_dispatcher::global().remove_window(*this); }

  void window::load(borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
                    float const w, float const h)
  {
    Ogre::TexturePtr texture{ Ogre::TextureManager::getSingleton().createManual(
        "ui_" + url,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Ogre::TEX_TYPE_2D, w, h, 0, Ogre::PF_A8R8G8B8, Ogre::TU_DYNAMIC_WRITE_ONLY) };

    Ogre::MaterialPtr const base_material{ Ogre::MaterialManager::getSingletonPtr()-> getByName("ortho_ui") };
    Ogre::MaterialPtr material{ base_material->clone("ortho_ui_" + url) };
    material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);

    Ogre::TextureUnitState * tex_unit{ material->getTechnique(0)->getPass(0)->createTextureUnitState() };
    tex_unit->setTextureName("ui_" + url, Ogre::TEX_TYPE_2D);
    tex_unit->setTextureCoordSet(0);

    auto obj(sm->createManualObject("ui_obj_" + url));
    auto const scale_x(w), scale_y(h);
    obj->setDynamic(false);
    obj->begin("ortho_ui_" + url, Ogre::RenderOperation::OT_TRIANGLE_STRIP);

    obj->position(0.0f, 0.0f, 0.0f);
    obj->textureCoord(0.0f, 0.0f);

    obj->position(0.0f, -scale_y, 0.0f);
    obj->textureCoord(0.0f, 1.0f);

    obj->position(scale_x, 0.0f, 0.0f);
    obj->textureCoord(1.0f, 0.0f);

    obj->position(scale_x, -scale_y, 0.0f);
    obj->textureCoord(1.0f, 1.0f);

    obj->end();

    auto const mesh(obj->convertToMesh("ui_mesh_" + url));
    m_entity = sm->createEntity(mesh);
    m_entity->setMaterial(material);

    Ogre::SceneNode * const node{ sm->getRootSceneNode()->createChildSceneNode("ui_node_" + url) };
    node->attachObject(m_entity);
    node->setPosition(400.0f, 0.0f, 0.0f);

    auto * const lis(new listener(texture));
    Ogre::Root::getSingletonPtr()->addFrameListener(lis);
    m_client = new client(lis);

    CefWindowInfo window_info;
    CefBrowserSettings browser_settings;
    window_info.SetAsOffScreen(nullptr);

    m_browser = CefBrowserHost::CreateBrowserSync(window_info, m_client.get(), 
                                                  url, browser_settings, nullptr);
  }

  void window::key_pressed(OIS::KeyEvent const &arg)
  {
  }

  void window::key_released(OIS::KeyEvent const &arg)
  {
  }

  void window::mouse_moved(OIS::MouseEvent const &arg)
  {
    CefMouseEvent ev;
    ev.x = arg.state.X.abs;
    ev.y = arg.state.Y.abs;
    ev.modifiers = 0;
    m_browser->GetHost()->SendMouseMoveEvent(ev, false);
  }

  static CefBrowserHost::MouseButtonType ois_to_cef_button(OIS::MouseButtonID const id)
  {
    switch(id)
    {
      case OIS::MB_Left:
        return MBT_LEFT;
      case OIS::MB_Right:
        return MBT_RIGHT;
      case OIS::MB_Middle:
        return MBT_MIDDLE;
      default:
        throw std::invalid_argument("Invalid button id: " + std::to_string(id));
    }
  }

  static void mouse_event(CefRefPtr<CefBrowserHost> const host, OIS::MouseEvent const &arg,
                     OIS::MouseButtonID const id, bool const up)
  {
    CefMouseEvent ev;
    ev.x = arg.state.X.abs;
    ev.y = arg.state.Y.abs;
    ev.modifiers = 0;
    host->SendMouseClickEvent(ev, ois_to_cef_button(id), up, 1);
  }

  void window::mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  { mouse_event(m_browser->GetHost(), arg, id, false); }

  void window::mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  { mouse_event(m_browser->GetHost(), arg, id, true); }
}
