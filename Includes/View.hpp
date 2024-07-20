#pragma once

#include "Runtime/Core.hpp"
#include "Runtime/Grid.hpp"

namespace Factory
{
    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        std::uint64_t getFPS(Chicane::Grid::ComponentEvent inEvent);
        std::string getFrametime(Chicane::Grid::ComponentEvent inEvent);

        int addActor(Chicane::Grid::ComponentEvent inEvent);
    };
}