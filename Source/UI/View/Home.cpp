#include "Ui/View/Home.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include <cstdlib>

namespace Factory
{
    HomeView::HomeView()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        ),
        m_currentDirectory("")
    {
        Chicane::Log::watchLogs(
            [this](const Chicane::Log::List& inLogs)
            {
                std::vector<std::any> logs;

                for (const Chicane::Log::Instance& log : inLogs)
                {
                    logs.push_back(std::make_any<Chicane::Log::Instance>(log));
                }

                m_logs = std::make_any<std::vector<std::any>>(logs);
            }
        );
        Chicane::watchActiveLevel(
            [this](Chicane::Level* inLevel)
            {
                if (!inLevel)
                {
                    updateOutline();
                }

                inLevel->watchActors(
                    [this](Chicane::Actor* inActor)
                    {
                        updateOutline();
                    }
                );
            }
        );

        listDir(".");

        addVariable(
            "actors",
            &m_actors
        );
        addVariable(
            "directoryHistory",
            &m_directoryHistory
        );
        addVariable(
            "directoryInfo",
            &m_directoryInfo
        );
        addVariable(
            "logs",
            &m_logs
        );

        addFunction(
            "getFPS",
            std::bind(&HomeView::getFPS, this, std::placeholders::_1)
        );
        addFunction(
            "getFrametime",
            std::bind(&HomeView::getFrametime, this, std::placeholders::_1)
        );
        addFunction(
            "showActor",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                showActor(std::any_cast<std::string>(inEvent.values[0]));

                return 0;
            }
        );
        addFunction(
            "showDirectoryHistory",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                showDirectoryHistory(std::any_cast<std::string>(inEvent.values[0]));

                return 0;
            }
        );
        addFunction(
            "showDirectory",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                showDirectory(std::any_cast<Chicane::FileSystem::ListItem>(inEvent.values[0]));

                return 0;
            }
        );
        addFunction(
            "showLog",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                showLog(std::any_cast<Chicane::Log::Instance>(inEvent.values[0]));

                return 0;
            }
        );
    }

    std::uint32_t HomeView::getFPS(const Chicane::Grid::ComponentEvent& inEvent)
    {
        return Chicane::getTelemetry().frame.rate;
    }

    std::string HomeView::getFrametime(const Chicane::Grid::ComponentEvent& inEvent)
    {
        std::string frametime = std::to_string(Chicane::getTelemetry().frame.time);

        return std::string(frametime.begin(), frametime.end() - 5);
    }

    void HomeView::showActor(const std::string& inActor)
    {
        Chicane::Grid::Style style {};
        Chicane::Grid::TextComponent::compileRaw(inActor, style);
    }

    void HomeView::showDirectoryHistory(const std::string& inPath)
    {
        std::vector<std::string> splittedPath = Chicane::Utils::split(
            inPath,
            Chicane::FileSystem::SEPARATOR
        );

        const std::string& folderName = splittedPath.back();

        if (
            ImGui::Button(
                folderName.c_str(),
                ImVec2(50, 20)
            )
        )
        {
            listDir(inPath);
        }
    }

    void HomeView::showDirectory(const Chicane::FileSystem::ListItem& inItem)
    {
        if (inItem.type == Chicane::FileSystem::ListType::Folder)
        {
            std::string title = inItem.name + " - " + std::to_string(inItem.childCount) + " Items";

            if (
                ImGui::Button(
                    title.c_str(),
                    ImVec2(200, 20)
                )
            )
            {
                listDir(inItem.path);
            }

            return;
        }

        if (
            ImGui::Button(
                inItem.name.c_str(),
                ImVec2(200, 20)
            )
        )
        {
            MeshActor* actor = new MeshActor(inItem.path);
            actor->setAbsoluteTranslation(
                Chicane::Vec<3, float>(
                    std::rand() % 100,
                    std::rand() % 100,
                    std::rand() % 10
                )
            );

            Chicane::addActor(actor);
        }
    }

    void HomeView::updateOutline()
    {
        std::unordered_map<std::string, std::uint32_t> ids {};

        std::vector<std::any> actors {};

        for (Chicane::Actor* actor : Chicane::getActors())
        {
            std::vector<std::string> splittedId = Chicane::Utils::split(
                typeid(*actor).name(),
                ':'
            );
            std::string id = splittedId.back();

            if (ids.find(id) == ids.end())
            {
                ids.insert(
                    std::make_pair(
                        id,
                        -1
                    )
                );
            }

            ids.at(id)++;

            actors.push_back(
                std::make_any<std::string>(id + "-" + std::to_string(ids.at(id)))
            );
        }

        m_actors = std::make_any<std::vector<std::any>>(actors);
    }

    void HomeView::updateDirHistory()
    {
        std::vector<std::any> items = {};

        std::vector<std::string> splittedPath = Chicane::Utils::split(
            m_currentDirectory,
            Chicane::FileSystem::SEPARATOR
        );

        for (std::uint32_t i = 0; i < splittedPath.size(); i++)
        {
            std::string path = "";
            
            for (std::uint32_t j = 0; j <= i; j++)
            {
                path += splittedPath.at(j);
                path += Chicane::FileSystem::SEPARATOR;
            }

            items.push_back(
                std::make_any<std::string>(path)
            );
        }

        m_directoryHistory = std::make_any<std::vector<std::any>>(items);
    }

    void HomeView::listDir(const std::string& inPath)
    {
        std::vector<std::any> items = {};

        for (const auto& item : Chicane::FileSystem::ls(inPath))
        {
            if (item.type == Chicane::FileSystem::ListType::Folder)
            {
                items.push_back(
                    std::make_any<Chicane::FileSystem::ListItem>(item)
                );

                continue;
            }

            std::string extension = item.extension;

            if (extension.compare(".box") != 0 && extension.compare(".grid") != 0)
            {
                continue;
            }

            items.push_back(
                std::make_any<Chicane::FileSystem::ListItem>(item)
            );
        }

        m_directoryInfo = std::make_any<std::vector<std::any>>(items);

        m_currentDirectory = inPath;

        updateDirHistory();
    }

    void HomeView::showLog(const Chicane::Log::Instance& inLog)
    {
        if (inLog.isEmpty())
        {
            return;
        }

        Chicane::Grid::Style style {};
        style.foregroundColor = inLog.color;

        Chicane::Grid::TextComponent::compileRaw(inLog.text, style);
    }
}