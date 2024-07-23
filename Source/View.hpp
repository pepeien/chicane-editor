#pragma once

#include "Base.hpp"

namespace Factory
{
    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        std::uint64_t getFPS(Chicane::Grid::ComponentEvent inEvent);
        std::string getFrametime(Chicane::Grid::ComponentEvent inEvent);

        int addFrontImage(Chicane::Grid::ComponentEvent inEvent);
        int addBehindImage(Chicane::Grid::ComponentEvent inEvent);
        int addLeftImage(Chicane::Grid::ComponentEvent inEvent);
        int addRightImage(Chicane::Grid::ComponentEvent inEvent);
        int addUpImage(Chicane::Grid::ComponentEvent inEvent);
        int addDownImage(Chicane::Grid::ComponentEvent inEvent);
        int onSubmit(Chicane::Grid::ComponentEvent inEvent);

    private:
        Chicane::FileSystem::FileFormat m_cubemapFileFormat;
        Chicane::Box::WriteInfo m_cubemapInfo;

        std::any m_name;
        std::any m_frontImage;
        std::any m_behindImage;
        std::any m_leftImage;
        std::any m_rightImage;
        std::any m_upImage;
        std::any m_downImage;
    };
}