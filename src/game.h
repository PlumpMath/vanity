#pragma once

#include <memory>
#include <cstdint>

#include <OgreManualObject.h>

#include "application.h"
#include "vox/fixed_volume.h"
#include "borrowed_ptr.h"

class game : public application
{
  public:
    game();
    ~game();

  protected:
    void create_scene() override;

    bool key_pressed(OIS::KeyEvent const &arg) override;
    bool key_released(OIS::KeyEvent const &arg) override;

    bool mouse_moved(OIS::MouseEvent const &arg) override;
    bool mouse_pressed(OIS::MouseEvent const &arg, OIS::MouseButtonID const id) override;
    bool mouse_released(OIS::MouseEvent const &arg, OIS::MouseButtonID const id) override;

    bool frame_rendering_queued(Ogre::FrameEvent const &evt) override;

  private:
    void update_surface();

    std::unique_ptr<vox::fixed_volume<uint8_t>> m_volume;
    borrowed_ptr<Ogre::ManualObject> m_ogre_volume{ nullptr };
    size_t m_unit_size{ 16 };
};
