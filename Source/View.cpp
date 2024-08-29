#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

#include <cstdlib>

namespace Factory
{
    class MeshActor : public Chicane::Actor
    {
    public:
        MeshActor(const std::string& inMesh)
            : Chicane::Actor(),
            m_mesh(std::make_unique<Chicane::MeshComponent>())
        {
            setAbsoluteTranslation(
                Chicane::Vec<3, float>(
                    std::rand() % 100,
                    std::rand() % 100,
                    std::rand() % 10
                )
            );

            m_mesh->attachTo(this);
            m_mesh->setMesh(inMesh);
            m_mesh->activate();
        }

    private:
        std::unique_ptr<Chicane::MeshComponent> m_mesh;
    };

    View::View()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        ),
        m_currentDirectory("")
    {
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

        addFunction(
            "getFPS",
            std::bind(&View::getFPS, this, std::placeholders::_1)
        );
        addFunction(
            "getFrametime",
            std::bind(&View::getFrametime, this, std::placeholders::_1)
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
    }

    std::uint32_t View::getFPS(const Chicane::Grid::ComponentEvent& inEvent)
    {
        return Chicane::getTelemetry().frame.rate;
    }

    std::string View::getFrametime(const Chicane::Grid::ComponentEvent& inEvent)
    {
        std::string frametime = std::to_string(Chicane::getTelemetry().frame.time);

        return std::string(frametime.begin(), frametime.end() - 5);
    }

    void View::showActor(const std::string& inActor)
    {
        Chicane::Grid::TextComponent::compileRaw(inActor);
    }

    void View::showDirectoryHistory(const std::string& inPath)
    {
        std::vector<std::string> tree = Chicane::Utils::split(inPath, '\\');

        for (int i = 0; i < tree.size(); i++)
        {
            const std::string& folder = tree.at(i);

            if (
                ImGui::Button(
                    folder.c_str(),
                    ImVec2(50, 20)
                )
            )
            {
                std::string path = "";

                for (int j = 0; j <= i; j++)
                {
                    path += tree.at(j);
                    path += "\\";
                }

                listDir(path);
            }

            ImGui::SameLine();

            Chicane::Grid::TextComponent::compileRaw("\\");

            ImGui::SameLine();
        }
    }

    void View::showDirectory(const Chicane::FileSystem::ListItem& inList)
    {
        if (inList.type != Chicane::FileSystem::ListType::Folder)
        {
            if (
                ImGui::Button(
                    inList.name.c_str(),
                    ImVec2(200, 20)
                )
            )
            {
                Chicane::addActor(new MeshActor(inList.path));
            }

            return;
        }

        std::string title = inList.name + " - " + std::to_string(inList.childCount) + " Items";

        if (
            ImGui::Button(
                title.c_str(),
                ImVec2(200, 20)
            )
        )
        {
            listDir(inList.path);
        }
    }

    void View::updateOutline()
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

    void View::updateDirHistory()
    {
        std::vector<std::any> items = {};

        items.push_back(
            std::make_any<std::string>(m_currentDirectory)
        );

        m_directoryHistory = std::make_any<std::vector<std::any>>(items);
    }

    void View::listDir(const std::string& inPath)
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
}