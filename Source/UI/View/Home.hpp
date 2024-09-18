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

        void showLog(const Chicane::Log::Instance& inLog);
        void showActor(Chicane::Actor* inActor);
        void showDirectoryHistory(const std::string& inPath);
        void showDirectory(const Chicane::FileSystem::ListItem& inList);

    private:
        void setupWatchers();

        void setupUiTelemetry();
        void setupUiActor();
        void setupUiDirectory();
        void setupUiConsole();

        void updateTranslation();
        void updateRotation();
        void updateScaling();
        void updateOutliner();
        void updateDirHistory();
        void listDir(const std::string& inPath);

    private:
        // Render
        std::any m_uiActors;
        std::any m_uiActorTranslationX;
        std::any m_uiActorTranslationY;
        std::any m_uiActorTranslationZ;
        std::any m_uiActorRotationX;
        std::any m_uiActorRotationY;
        std::any m_uiActorRotationZ;
        std::any m_uiActorScalingX;
        std::any m_uiActorScalingY;
        std::any m_uiActorScalingZ;

        std::any m_uiDirectoryHistory;
        std::any m_uiDirectoryInfo;

        std::any m_uiIsConsoleOpen;
        std::any m_uiConsoleLogs;

        // Internal
        std::string m_currentDirectory;
        Chicane::Actor* m_selectedActor;
    };
}