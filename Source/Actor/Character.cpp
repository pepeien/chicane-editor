#include "Actor/Character.hpp"

#include "Base.hpp"

static constexpr float MOVE_COEFFICIENT = 0.25f;

namespace Chicane
{
    AEditorCharacter::AEditorCharacter()
        : ACharacter(),
        m_cameraComponent(nullptr)
    {
        m_cameraComponent = Application::getLevel()->createComponent<CCamera>();
        m_cameraComponent->attachTo(this);
        m_cameraComponent->activate();
    }

    void AEditorCharacter::onControlAttachment()
    {
        // Mouse
        m_controller->bindEvent(std::bind(&AEditorCharacter::onLook, this, std::placeholders::_1));

        m_controller->bindEvent(
            Controller::MouseButton::Left,
            Controller::EventStatus::Pressed,
            [&]()
            {
                if (Application::getWindow()->isFocused())
                {
                    return;
                }

                float mouseX = 0.0f;
                float mouseY = 0.0f;
                SDL_GetMouseState(&mouseX, &mouseY);

                std::uint32_t width  = static_cast<std::uint32_t>(Grid::getSize("82vw"));
                std::uint32_t height = static_cast<std::uint32_t>(Grid::getSize("80vh"));

                if (mouseX > width || mouseY > height)
                {
                    return;
                }

                Application::getWindow()->focus();
            }
        );

        m_controller->bindEvent(
            Controller::MouseButton::Right,
            Controller::EventStatus::Pressed,
            [&]()
            {
                if (!Application::getWindow()->isFocused())
                {
                    return;
                }

                Application::getWindow()->blur();
            }
        );

        // Keyboard
        m_controller->bindEvent(
            Controller::KeyboardKey::W,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveForward, this)
        );
        m_controller->bindEvent(
            Controller::KeyboardKey::S,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveBackward, this)
        );
        m_controller->bindEvent(
            Controller::KeyboardKey::A,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveLeft, this)
        );
        m_controller->bindEvent(
            Controller::KeyboardKey::D,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveRight, this)
        );
        m_controller->bindEvent(
            Controller::KeyboardKey::Space,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveUp, this)
        );
        m_controller->bindEvent(
            Controller::KeyboardKey::Lctrl,
            Controller::EventStatus::Pressed,
            std::bind(&AEditorCharacter::onMoveDown, this)
        );
    }

    void AEditorCharacter::onLook(const Controller::MouseMotionEvent& inEvent)
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        addAbsoluteRotation(-inEvent.yrel * 0.5f, 0.0f, -inEvent.xrel * 0.5f);
    }

    void AEditorCharacter::onMoveForward()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(getForward(), MOVE_COEFFICIENT);
    }

    void AEditorCharacter::onMoveBackward()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(getForward(), -MOVE_COEFFICIENT);
    }

    void AEditorCharacter::onMoveLeft()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(getRight(), -MOVE_COEFFICIENT);
    }

    void AEditorCharacter::onMoveRight()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(getRight(), MOVE_COEFFICIENT);
    }

    void AEditorCharacter::onMoveUp()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(UP_DIRECTION, MOVE_COEFFICIENT);
    }

    void AEditorCharacter::onMoveDown()
    {
        if (!Application::getWindow()->isFocused())
        {
            return;
        }

        move(UP_DIRECTION, -MOVE_COEFFICIENT);
    }
}