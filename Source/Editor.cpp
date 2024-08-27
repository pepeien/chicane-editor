#include "Editor.hpp"

#include "Layer.hpp"
#include "View.hpp"

#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Factory
{
    class DefaultActor : public Chicane::Actor
    {
    public:
        DefaultActor()
            : Chicane::Actor(),
            m_mesh(std::make_unique<Chicane::MeshComponent>())
        {
            m_mesh->attachTo(this);
            m_mesh->setMesh("Content/Meshes/Cube.box");
            m_mesh->activate();
        }

    private:
        std::unique_ptr<Chicane::MeshComponent> m_mesh;
    };

    void run()
    {
        Chicane::Allocator::load("Content/Textures/Skybox/Gray.box");

        std::unique_ptr<Chicane::Controller> controller = std::make_unique<Chicane::Controller>();
        Chicane::setActiveController(controller.get());

        std::unique_ptr<Chicane::Level> level = std::make_unique<Chicane::Level>();
        Chicane::setActiveLevel(level.get());

        Chicane::CameraActor* character = new Chicane::CameraActor();
        character->setAbsoluteTranslation(Chicane::Vec<3, float>(-2.5f, -10.0f, 5.0f));
        character->setAbsoluteRotation(Chicane::Vec<3, float>(0.0f, -10.0f, -25.0f));
        Chicane::addActor(character);
        Chicane::getActiveController()->attachTo(character);

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
