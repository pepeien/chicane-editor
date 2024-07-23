#pragma once

#include "Base.hpp"

namespace Factory
{
    class Camera : public Chicane::Camera
    {
    public:
        Camera();

    public:
        void onEvent(const SDL_Event& inEvent) override;
    };
}