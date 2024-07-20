#include "View.hpp"

#include "Runtime/Core.hpp"
#include "Runtime/Game/State.hpp"

namespace Factory
{
    View::View()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        )
    {
        addFunction(
            "getFPS",
            std::bind(&View::getFPS, this, std::placeholders::_1)
        );
        addFunction(
            "getFrametime",
            std::bind(&View::getFrametime, this, std::placeholders::_1)
        );
        addFunction(
            "addActor",
            std::bind(&View::addActor, this, std::placeholders::_1)
        );
    }

    std::uint64_t View::getFPS(Chicane::Grid::ComponentEvent inEvent)
    {
        return Chicane::State::getTelemetry().framerate;
    }

    std::string View::getFrametime(Chicane::Grid::ComponentEvent inEvent)
    {
        std::string frametime = std::to_string(Chicane::State::getTelemetry().time);

        return std::string(frametime.begin(), frametime.end() - 5);
    }

    int View::addActor(Chicane::Grid::ComponentEvent inEvent)
    {
        return 0;
    }
}