#include "Camera.hpp"

namespace Factory
{
    void Camera::onEvent(const SDL_Event& inEvent)
    {
        if (inEvent.type != SDL_MOUSEMOTION)
        {
            return;
        }

        switch (inEvent.motion.state)
        {
        case SDL_BUTTON_LMASK:
            rotate(
                glm::vec2(
                    inEvent.motion.xrel,
                    inEvent.motion.yrel
                )
            );

            break;
        case SDL_BUTTON_MMASK:
            zoom(inEvent.motion.yrel);

            break;

        case SDL_BUTTON_RMASK:
            pan(
                glm::vec2(
                    inEvent.motion.xrel,
                    inEvent.motion.yrel
                )
            );

            break;

        default:
            break;
        }
    }
}