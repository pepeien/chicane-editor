#include "Editor.hpp"

#include "Layer.hpp"
#include "UI/View/Home.hpp"

namespace Chicane
{
    namespace Editor
    {
        void run()
        {
            std::unique_ptr<Level> level;
            std::unique_ptr<HomeView> view;
            std::unique_ptr<Controller::Instance> controller;

            Application::CreateInfo createInfo = {};
            createInfo.window.title   = "Chicane Editor";
            createInfo.window.icon    = "Content/Icon.png";
            createInfo.window.size    = Vec<2, int>(1600, 900);
            createInfo.window.display = 0;
            createInfo.window.type   = Window::Type::Windowed;
            createInfo.renderer.type = Renderer::Type::Vulkan;
            createInfo.onSetup = [&]()
            {
                // Controller
                controller = std::make_unique<Controller::Instance>();
                Application::setController(controller.get());

                // Level
                level = std::make_unique<Level>();
                level->activate();

                level->createActor<ASky>()->setSky(
                    Box::loadSky("Content/Skies/Gray.bsky")
                );

                level->createActor<ALight>();

                ACharacter* character = level->createActor<ACharacter>();
                character->attachController(Application::getController());
                character->setAbsoluteTranslation(Vec<3, float>(-5.0f, -7.0f, 2.0f));
                character->setAbsoluteRotation(Vec<3, float>(0.0f, -34.5f, -12.5f));

                CCamera* camera = level->createComponent<CCamera>();
                camera->attachTo(character);
                camera->activate();

                // UI
                view = std::make_unique<HomeView>();
                Application::addView(view.get());
                Application::setView(view->getId());

                // Grid
                Application::getRenderer()->pushLayer(
                    new LGrid(),
                    Chicane::Layer::PushStrategy::AfterLayer,
                    "Level"
                );
            };

            Application::run(createInfo);
        }
    }
}
