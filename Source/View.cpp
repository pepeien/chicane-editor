#include "View.hpp"

#include "Chicane/Core.hpp"
#include "Chicane/Game.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Factory
{
    class MeshActor : public Chicane::Actor
    {
    public:
        MeshActor(const std::string& inMesh)
            : Chicane::Actor(),
            m_mesh(std::make_unique<Chicane::MeshComponent>())
        {
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
        listDir(".");

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
            "showDirectoryHistory",
            std::bind(&View::showDirectoryHistory, this, std::placeholders::_1)
        );
        addFunction(
            "showDirectory",
            std::bind(&View::showDirectory, this, std::placeholders::_1)
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

    int View::showDirectoryHistory(const Chicane::Grid::ComponentEvent& inEvent)
    {
        std::string item = std::any_cast<std::string>(inEvent.values[0]);

        std::vector<std::string> tree = Chicane::Utils::split(item, '\\');

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

        return 0;
    }

    int View::showDirectory(const Chicane::Grid::ComponentEvent& inEvent)
    {
        auto item = std::any_cast<Chicane::FileSystem::ListItem>(inEvent.values[0]);

        if (item.type != Chicane::FileSystem::ListType::Folder)
        {
            if (
                ImGui::Button(
                    item.name.c_str(),
                    ImVec2(200, 20)
                )
            )
            {
                Chicane::addActor(new MeshActor(item.path));
            }

            return 0;
        }

        std::string title = item.name + " - " + std::to_string(item.childCount) + " Items";

        if (
            ImGui::Button(
                title.c_str(),
                ImVec2(200, 20)
            )
        )
        {
            listDir(item.path);
        }

        return 0;
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