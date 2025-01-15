#pragma once

#include "Base.hpp"

namespace Chicane
{
    namespace Editor
    {
        class HomeView : public Grid::View
        {
        public:
            HomeView();

        private:
            Grid::Reference getFPS(const Grid::Component::Event& inEvent);
            Grid::Reference getFrametime(const Grid::Component::Event& inEvent);

            void showLog(const Log::Entry* inLog);
            void showActor(const Actor* inActor);
            void showDirectoryHistory(const std::string& inPath);
            void showDirectory(const FileSystem::Item& inList);

        private:
            void setupWatchers();

            void setupUiTelemetry();
            void setupUiActor();
            void setupUiDirectory();
            void setupUiAssetCreator();
            void setupUiConsole();

            void updateTranslation();
            void updateRotation();
            void updateScaling();
            void updateOutliner();
            void updateDirHistory(const std::string& inPath);
            void listDir(const std::string& inPath);

        private:
            // Internal
            std::vector<std::string>      m_directoryHistory;
            std::vector<Grid::Reference>  m_directoryHistoryRefs;
            std::vector<FileSystem::Item> m_directoryInfo;
            std::vector<Grid::Reference>  m_directoryInfoRefs;

            std::vector<Actor*>           m_actors;
            const Actor*                  m_actor;
            Transform                     m_actorTransform;

            bool                          m_bIsConsoleOpen;
            std::vector<Grid::Reference>  m_consoleLogs;

            // UI
            Grid::Reference               m_uiDirectoryHistory;
            Grid::Reference               m_uiDirectoryInfo;

            Grid::Reference               m_uiActors;
            Grid::Reference               m_uiSelectedActorTranslation;
            Grid::Reference               m_uiSelectedActorRotation;
            Grid::Reference               m_uiSelectedActorScale;

            Grid::Reference               m_uiIsConsoleOpen;
            Grid::Reference               m_uiConsoleLogs;
        };
    }
}