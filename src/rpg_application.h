#pragma once

#include "application.h"

class rpg_application : public application
{
  public:
    rpg_application();
    virtual ~rpg_application();

  protected:
    virtual void create_scene() override;
};
