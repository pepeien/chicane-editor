#include "Camera.hpp"

#include "Runtime/Runtime.hpp"

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
            rotateTo(
                glm::vec2(
                    inEvent.motion.xrel,
                    inEvent.motion.yrel
                )
            );

            break;
        case SDL_BUTTON_MMASK:
            zoomTo(inEvent.motion.yrel);

            break;

        case SDL_BUTTON_RMASK:
            panTo(
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