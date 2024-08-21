#pragma once

#include "Base.hpp"

namespace Factory
{
    class Actor : public Chicane::Actor
    {
    public:
        Actor();
    };

    class View : public Chicane::Grid::View
    {
    public:
        View();

    private:
        std::uint32_t getFPS(const Chicane::Grid::ComponentEvent& inEvent);
        std::string getFrametime(const Chicane::Grid::ComponentEvent& inEvent);
        int showDirectoryHistory(const Chicane::Grid::ComponentEvent& inEvent);
        int showDirectory(const Chicane::Grid::ComponentEvent& inEvent);

    private:
        void updateDirHistory();
        void listDir(const std::string& inPath);

    private:
        // Render
        std::any m_directoryHistory;
        std::any m_directoryInfo;

        // Internal
        std::string m_currentDirectory;
    };
}