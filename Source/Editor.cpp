#include "Editor.hpp"

#include "Camera.hpp"
#include "Layer.hpp"
#include "View.hpp"

namespace Factory
{
    void run()
    {
        std::unique_ptr<Camera> camera = std::make_unique<Camera>();
        Chicane::setCamera(camera.get());

        Chicane::Allocator::load("Content/Textures/Skybox/Gray.box");

        std::unique_ptr<Chicane::Level> level = std::make_unique<Chicane::Level>();
        Chicane::setLevel(level.get());

        std::unique_ptr<View> view = std::make_unique<View>();
        Chicane::Grid::addView(view.get());
        Chicane::Grid::setActiveView(view->getId());

        Chicane::WindowCreateInfo windowCreateInfo = {};
        windowCreateInfo.title         = "Factory Editor";
        windowCreateInfo.icon          = "Content/Icon.png";
        windowCreateInfo.resolution.x  = 1600;
        windowCreateInfo.resolution.y  = 900;
        windowCreateInfo.type          = Chicane::WindowType::Windowed;
        windowCreateInfo.displayIndex  = 0;

        std::unique_ptr<Chicane::Window> window = std::make_unique<Chicane::Window>(windowCreateInfo);
        window->addLayer(
            new Layer(window.get()),
            Chicane::Layer::Push::BeforeLayer,
            "Level"
        );
        window->run();
    }
}
