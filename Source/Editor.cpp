#include "Editor.hpp"

#include "Layer.hpp"
#include "UI/View/Home.hpp"

namespace Chicane
{
    namespace Editor
    {
        void run()
        {
            std::unique_ptr<HomeView> view;

            Application::CreateInfo createInfo = {};
            createInfo.title        = "Chicane Editor";
            createInfo.icon         = "Content/Icon.png";
            createInfo.resolution.x = 1600;
            createInfo.resolution.y = 900;
            createInfo.window       = Window::Type::Windowed;
            createInfo.renderer     = Renderer::Type::Vulkan;
            createInfo.display      = 0;
            createInfo.onSetup      = [&]()
            {
                view = std::make_unique<HomeView>();

                Application::getRenderer()->pushLayer(
                    new Layer(),
                    Chicane::Layer::PushStrategy::AfterLayer,
                    "Level"
                );

                Loader::loadCubemap("Content/CubeMaps/Gray.bcmp");

                Application::addView(view.get());
                Application::setView(view->getId());

                CameraPawn* character = new CameraPawn();
                character->setAbsoluteTranslation(Vec<3, float>(-5.0f, -7.0f, 2.0f));
                character->setAbsoluteRotation(Vec<3, float>(0.0f, -34.5f, -12.5f));
                Application::getLevel()->addActor(character);
                Application::getController()->attachTo(character);
            };

            Application::run(createInfo);
        }
    }
}
