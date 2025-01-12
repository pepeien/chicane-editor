#include "Editor.hpp"

#include "Layer.hpp"
#include "UI/View/Home.hpp"

namespace Editor
{
    void run()
    {
        std::unique_ptr<HomeView> view;

        Chicane::Application::CreateInfo createInfo = {};
        createInfo.title        = "Chicane Editor";
        createInfo.icon         = "Content/Icon.png";
        createInfo.resolution.x = 1600;
        createInfo.resolution.y = 900;
        createInfo.window       = Chicane::Window::Type::Windowed;
        createInfo.renderer     = Chicane::Renderer::Type::Vulkan;
        createInfo.display      = 0;
        createInfo.onSetup      = [&]()
        {
            view = std::make_unique<HomeView>();

            Chicane::Application::getRenderer()->pushLayer(
                new Layer(),
                Chicane::Layer::PushStrategy::AfterLayer,
                "Level"
            );

            Chicane::Loader::loadCubemap("Content/CubeMaps/Gray.bcmp");

            Chicane::Application::addView(view.get());
            Chicane::Application::setView(view->getId());

            Chicane::CameraPawn* character = new Chicane::CameraPawn();
            character->setAbsoluteTranslation(Chicane::Vec<3, float>(-5.0f, -7.0f, 2.0f));
            character->setAbsoluteRotation(Chicane::Vec<3, float>(0.0f, -34.5f, -12.5f));
            Chicane::Application::getLevel()->addActor(character);
            Chicane::Application::getController()->attachTo(character);
        };

        Chicane::Application::run(createInfo);
    }
}
