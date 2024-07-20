#pragma once

#include "Runtime/Renderer.hpp"

namespace Factory
{
    class Camera : public Chicane::Camera
    {
        void onEvent(const SDL_Event& inEvent) override;
    };
}