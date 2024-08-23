#include "Editor.hpp"

#include "Layer.hpp"
#include "View.hpp"

namespace Factory
{
    class DefaultActor : public Chicane::Actor
    {
    public:
        DefaultActor()
            : Chicane::Actor()
        {
            setAbsoluteScale(Chicane::Math<float, 3>::Vec(0.15f));

            m_mesh = Chicane::Allocator::load("Content/Meshes/Aircraft.box");
        }
    };

    void run()
    {
        Chicane::Allocator::load("Content/Textures/Skybox/Gray.box");

        std::unique_ptr<Chicane::Controller> controller = std::make_unique<Chicane::Controller>();
        Chicane::setController(controller.get());

        std::unique_ptr<Chicane::Level> level = std::make_unique<Chicane::Level>();
        Chicane::setLevel(level.get());

        Chicane::CameraActor* character = new Chicane::CameraActor();
        Chicane::addActor(character);
        Chicane::getController()->attachTo(character);

        Chicane::addActor(new DefaultActor());

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
