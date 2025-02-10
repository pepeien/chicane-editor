#include "Editor.hpp"

#include "Actor/Character.hpp"
#include "Layer/Grid.hpp"
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
            createInfo.window.type    = Window::Type::Windowed;

            createInfo.renderer.type = Renderer::Type::Vulkan;

            createInfo.onSetup = [&]()
            {
                // Controller
                controller = std::make_unique<Controller::Instance>();
                Application::setController(controller.get());

                // Level
                level = std::make_unique<Level>();
                level->activate();

                ASky* skybox = level->createActor<ASky>();
                skybox->setSky(Box::loadSky("Content/Skies/Gray.bsky"));

                ALight* globalLight = level->createActor<ALight>();
                globalLight->setAbsoluteTranslation(0.0f, 0.0f, 1000.0f);

                AEditorCharacter* character = level->createActor<AEditorCharacter>();
                character->attachController(Application::getController());
                character->setAbsoluteTranslation(-5.0f, -14.0f, 5.0f);
                character->setAbsoluteRotation(-6.0f, 0.0f, 28.0f);

                // UI
                view = std::make_unique<HomeView>();
                Application::addView(view.get());
                Application::setView(view->getId());

                // Layer
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
