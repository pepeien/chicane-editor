#include "Editor.hpp"

#include <filesystem>

#include "Runtime/Grid.hpp"

#include "Camera.hpp"
#include "View.hpp"
#include "Layer.hpp"

namespace Factory
{
    void run()
    {
        std::unique_ptr<Camera> camera = std::make_unique<Camera>();
        Chicane::State::setCamera(camera.get());

        std::unique_ptr<Chicane::Level> level = std::make_unique<Chicane::Level>();
        level->setSkybox(Chicane::Box::read("Content/Textures/SKY_Gray.box"));
        Chicane::State::setLevel(level.get());

        std::unique_ptr<View> view = std::make_unique<View>();
        Chicane::Grid::addView(view.get());
        Chicane::Grid::setActiveView(view->getId());

        Chicane::WindowCreateInfo windowCreateInfo = {};
        windowCreateInfo.title         = "Factory Editor";
        windowCreateInfo.resolution.x  = 1600;
        windowCreateInfo.resolution.y  = 900;
        windowCreateInfo.type          = Chicane::WindowType::Windowed;
        windowCreateInfo.displayIndex  = 1;

        std::unique_ptr<Chicane::Window> window = std::make_unique<Chicane::Window>(windowCreateInfo);
        window->addLayer(
            new Layer(window.get()),
            Chicane::LayerPushTecnique::BeforeLayer,
            "Level"
        );
        window->run();
    }
}
