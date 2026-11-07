#pragma once

#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "../MultiplayUI/multiplayUI.h"

extern MultiplayUI multiplayUI;

class Multi
{
public:
    std::string message;

    void render_multi();
};