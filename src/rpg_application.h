#pragma once

#include <memory>
#include <cstdint>

#include <OgreManualObject.h>

#include "application.h"

#include "vox/fixed_volume.h"

class rpg_application : public application
{
  public:
    rpg_application();
    virtual ~rpg_application();

  protected:
    virtual void create_scene() override;
    bool keyPressed(OIS::KeyEvent const &arg) override;

  private:
    void update_surface();

    std::unique_ptr<vox::fixed_volume<uint8_t>> m_volume;
    Ogre::ManualObject *m_ogre_volume{ nullptr };
    size_t m_unit_size{ 16 };
};
