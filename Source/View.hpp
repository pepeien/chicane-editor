#pragma once

#include "Base.hpp"

namespace Factory
{
    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        std::uint32_t getFPS(Chicane::Grid::ComponentEvent inEvent);
        std::string getFrametime(Chicane::Grid::ComponentEvent inEvent);
        int onSelectImage(Chicane::Grid::ComponentEvent inEvent);
        int onSubmit(Chicane::Grid::ComponentEvent inEvent);

    private:
        std::any m_name;
        std::any m_imagePath;
    };
}