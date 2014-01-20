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
#include "surface.h"
#include "server.h"
#include "log/logger.h"
#include "dispatch/sender.h"

#include <Awesomium/STLHelpers.h>

#include <OgreMeshManager.h>
#include <OgreManualObject.h>

#include <chrono>
#include <thread>
#include <array>

namespace ui
{
  /* Helper functions for OIS -> Awesomium input. */
  static int convert_key(OIS::KeyCode const code);
  static bool is_special_key(OIS::KeyCode const code);
  static Awesomium::WebKeyboardEvent key_event(OIS::KeyEvent const &arg, bool const down);
  static Awesomium::MouseButton convert_mouse_button(OIS::MouseButtonID const id);

  window::window(borrowed_ptr<server> serv, borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
                 float const w, float const h)
    : m_url(url)
    , m_server(serv)
    , m_web_view(nullptr, std::mem_fn(&Awesomium::WebView::Destroy))
  {
    load(sm, url, w, h);
    input_dispatcher::global().add_window(*this);
  }

  window::~window()
  { input_dispatcher::global().remove_window(*this); }

  void window::load(borrowed_ptr<Ogre::SceneManager> sm, std::string const &url,
                    float const w, float const h)
  {
    m_web_view.reset(m_server->get_web_core()->CreateWebView(w, h, nullptr,
                                                             Awesomium::kWebViewType_Offscreen));

    Awesomium::WebURL const web_url{ Awesomium::WSLit(url.c_str()) };
    m_web_view->LoadURL(web_url);

    /* Setup the JS system for this window (globals, functions, etc). */
    init_js();

    while(m_web_view->IsLoading())
    { m_server->get_web_core()->Update(); }

    m_web_view->set_js_method_handler(&m_reciever);

    m_surface = static_cast<surface*>(m_web_view->surface());

    Ogre::MaterialPtr const base_material{ Ogre::MaterialManager::getSingletonPtr()->getByName("ortho_ui") };
    Ogre::MaterialPtr material{ base_material->clone("ortho_ui_" + url) };
    material->getTechnique(0)->getPass(0)->setCullingMode(Ogre::CULL_NONE);

    Ogre::TextureUnitState * tex_unit{ material->getTechnique(0)->getPass(0)->createTextureUnitState() };
    tex_unit->setTextureName("ui_" + std::to_string(42), Ogre::TEX_TYPE_2D);
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
  }

  void window::init_js()
  {
    m_global_obj = m_web_view->CreateGlobalJavascriptObject(Awesomium::WSLit("vanity"));
    m_win_obj = m_web_view->CreateGlobalJavascriptObject(Awesomium::WSLit("vanity.window"));

    /* EXAMPLE */
    /* Subscribe (create a new function on an object). */
    std::function<void ()> const get_url([&]()
    { log_debug("window url is: %%", Awesomium::ToString(m_web_view->url().spec())); });
    m_reciever.subscribe(m_win_obj.ToObject(), "get_url", get_url);

    /* Invoke (call a function on an object, may or may not be C++). */
    dispatch::sender::post(borrowed_ptr<Awesomium::WebView>{m_web_view.get()},
                           "vanity.window.get_url");
  }

  void window::key_pressed(OIS::KeyEvent const &arg)
  { m_web_view->InjectKeyboardEvent(key_event(arg, true)); }

  void window::key_released(OIS::KeyEvent const &arg)
  { m_web_view->InjectKeyboardEvent(key_event(arg, false)); }

  void window::mouse_moved(OIS::MouseEvent const &arg)
  {
    if(arg.state.Z.rel)
    { m_web_view->InjectMouseWheel(arg.state.Z.rel, 0); }
    else
    { m_web_view->InjectMouseMove(arg.state.X.abs, arg.state.Y.abs); }
  }

  void window::mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  { m_web_view->InjectMouseDown(convert_mouse_button(id)); }

  void window::mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id)
  { m_web_view->InjectMouseUp(convert_mouse_button(id)); }

  void window::focus()
  { m_web_view->Focus(); }

  void window::unfocus()
  { m_web_view->Unfocus(); }

  static int convert_key(OIS::KeyCode const code)
  {
    switch(code)
    {
      case OIS::KC_LEFT:
        return Awesomium::KeyCodes::AK_LEFT;
      case OIS::KC_RIGHT:
        return Awesomium::KeyCodes::AK_RIGHT;
      case OIS::KC_UP:
        return Awesomium::KeyCodes::AK_UP;
      case OIS::KC_DOWN:
        return Awesomium::KeyCodes::AK_DOWN;

      case OIS::KC_BACK:
        return Awesomium::KeyCodes::AK_BACK;
      case OIS::KC_DELETE:
        return Awesomium::KeyCodes::AK_DELETE;

      case OIS::KC_PGUP:
        return Awesomium::KeyCodes::AK_PRIOR;
      case OIS::KC_PGDOWN:
        return Awesomium::KeyCodes::AK_NEXT;
      case OIS::KC_HOME:
        return Awesomium::KeyCodes::AK_HOME;
      case OIS::KC_END:
        return Awesomium::KeyCodes::AK_END;

      default:
        return 0;
    }
  }

  static bool is_special_key(OIS::KeyCode const code)
  {
    static OIS::KeyCode const keys[] =
    {
      OIS::KC_LEFT, OIS::KC_RIGHT, OIS::KC_UP, OIS::KC_DOWN,
      OIS::KC_BACK, OIS::KC_PGUP, OIS::KC_PGDOWN, OIS::KC_HOME, OIS::KC_END,
      OIS::KC_DELETE
    };

    for(auto key : keys)
    {
      if(key == code)
      { return true; }
    }

    return false;
  }

  static Awesomium::WebKeyboardEvent key_event(OIS::KeyEvent const &arg, bool const down)
  {
    Awesomium::WebKeyboardEvent ev{};
    ev.type = down ? Awesomium::WebKeyboardEvent::kTypeKeyDown :
                     Awesomium::WebKeyboardEvent::kTypeKeyUp;
    ev.virtual_key_code = convert_key(arg.key);

    if(down && !is_special_key(arg.key))
    {
      ev.type = Awesomium::WebKeyboardEvent::kTypeChar;
      ev.text[0] = static_cast<char>(arg.text);
      ev.virtual_key_code = 0;
    }

    std::array<char, 20> buf;
    auto *data(buf.data());
    Awesomium::GetKeyIdentifierFromVirtualKeyCode(ev.virtual_key_code, static_cast<char**>(&data));
    std::strcpy(ev.key_identifier, data);

    ev.modifiers = 0;
    ev.unmodified_text[0] = ev.text[0];

    ev.native_key_code = 0;
    ev.is_system_key = false;

    return ev;
  }

  static Awesomium::MouseButton convert_mouse_button(OIS::MouseButtonID const id)
  {
    switch(id)
    {
      case OIS::MB_Left:
        return Awesomium::kMouseButton_Left;
      case OIS::MB_Middle:
        return Awesomium::kMouseButton_Middle;
      case OIS::MB_Right:
        return Awesomium::kMouseButton_Right;
      default:
        throw std::invalid_argument("Incompatible mouse button");
    }
  }
}
