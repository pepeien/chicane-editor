#pragma once

#include "Base.hpp"

namespace Factory
{
    class HomeView : public Chicane::Grid::View
    {
    public:
        HomeView();

    private:
        std::uint32_t getFPS(const Chicane::Grid::ComponentEvent& inEvent);
        std::string getFrametime(const Chicane::Grid::ComponentEvent& inEvent);

        void showActor(const std::string& inActor);
        void showDirectoryHistory(const std::string& inPath);
        void showDirectory(const Chicane::FileSystem::ListItem& inList);

    private:
        void updateOutline();
        void updateDirHistory();
        void listDir(const std::string& inPath);
        void showLog(const Chicane::Log::Instance& inLog);

    private:
        // Render
        std::any m_actors;
        std::any m_directoryHistory;
        std::any m_directoryInfo;
        std::any m_logs;

        // Internal
        std::string m_currentDirectory;
    };
}