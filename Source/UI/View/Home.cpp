#include "UI/View/Home.hpp"

#include "Base.hpp"
#include "Chicane/Game/Actor/Component/Mesh.hpp"

namespace Factory
{
    HomeView::HomeView()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        ),
        m_isConsoleOpen(std::make_any<std::string>("false")),
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

                m_consoleLogs = std::make_any<std::vector<std::any>>(logs);
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
            "consoleLogs",
            &m_consoleLogs
        );
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
            "isConsoleOpen",
            &m_isConsoleOpen
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
            "showConsole",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                m_isConsoleOpen = std::make_any<std::string>("true");

                return 0;
            }
        );
        addFunction(
            "hideConsole",
            [this](const Chicane::Grid::ComponentEvent& inEvent)
            {
                m_isConsoleOpen = std::make_any<std::string>("false");

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

    std::uint32_t HomeView::getFPS(const Chicane::Grid::ComponentEvent& inEvent)
    {
        return Chicane::getTelemetry().frame.rate;
    }

    std::string HomeView::getFrametime(const Chicane::Grid::ComponentEvent& inEvent)
    {
        std::string frametime = std::to_string(Chicane::getTelemetry().frame.time);

        return std::string(frametime.begin(), frametime.end() - 5);
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

    void HomeView::showActor(const std::string& inActor)
    {
        Chicane::Grid::ContainerComponent::Props props {};
        props.id           = "outlineWrapper-" + inActor;
        props.style.width  = Chicane::Grid::getSize("100%");
        props.style.height = Chicane::Grid::getSize("2vh");
        props._renderers.push_back(
            [inActor](const Chicane::Grid::ComponentEvent& inEvent)
            {
                Chicane::Grid::Style style {};
                style.horizontalAlignment = Chicane::Grid::Alignment::Center;
                style.verticalAlignment   = Chicane::Grid::Alignment::Center;

                Chicane::Grid::TextComponent::compileRaw(inActor, style);

                return 0;
            }
        );

        Chicane::Grid::ContainerComponent::compileRaw(props);
    }

    void HomeView::showDirectoryHistory(const std::string& inPath)
    {
        std::vector<std::string> splittedPath = Chicane::Utils::split(
            inPath,
            Chicane::FileSystem::SEPARATOR
        );

        const std::string& folderName = splittedPath.back();

        Chicane::Grid::ButtonComponent::Props props {};
        props.id = inPath;
        props.style.width = Chicane::Grid::getSize(
            "5vw",
            Chicane::Grid::Direction::Horizontal,
            Chicane::Grid::Position::Relative
        );
        props.style.height = Chicane::Grid::getSize(
            Chicane::Grid::AUTO_SIZE_UNIT,
            Chicane::Grid::Direction::Vertical,
            Chicane::Grid::Position::Relative
        );
        props.style.backgroundColor = "#444444";
        props.onClick = [&](const Chicane::Grid::ComponentEvent& inEvent)
        {
            listDir(inPath);

            return 0;
        };
        props._renderers.push_back(
            [&](const Chicane::Grid::ComponentEvent& inEvent)
            {
                Chicane::Grid::Style style {};
                style.horizontalAlignment = Chicane::Grid::Alignment::Center;
                style.verticalAlignment   = Chicane::Grid::Alignment::Center;

                Chicane::Grid::TextComponent::compileRaw(folderName, style);

                return 0;
            }
        );

        Chicane::Grid::ButtonComponent::compileRaw(props);
    }

    void HomeView::showDirectory(const Chicane::FileSystem::ListItem& inItem)
    {
        Chicane::Grid::ButtonComponent::Props props {};
        props.id = inItem.path;
        props.style.width = Chicane::Grid::getSize(
            Chicane::Grid::AUTO_SIZE_UNIT,
            Chicane::Grid::Direction::Horizontal,
            Chicane::Grid::Position::Relative
        );
        props.style.height = Chicane::Grid::getSize(
            Chicane::Grid::AUTO_SIZE_UNIT,
            Chicane::Grid::Direction::Vertical,
            Chicane::Grid::Position::Relative
        );
        props.style.backgroundColor = "#444444";

        if (inItem.type == Chicane::FileSystem::ListType::Folder)
        {
            props.onClick = [&](const Chicane::Grid::ComponentEvent& inEvent)
            {
                listDir(inItem.path);

                return 0;
            };
            props._renderers.push_back(
                [&](const Chicane::Grid::ComponentEvent& inEvent)
                {
                    Chicane::Grid::Style style {};
                    style.horizontalAlignment = Chicane::Grid::Alignment::Center;
                    style.verticalAlignment   = Chicane::Grid::Alignment::Center;

                    std::string title = inItem.name + " - " + std::to_string(inItem.childCount) + " Items";

                    Chicane::Grid::TextComponent::compileRaw(title, style);

                    return 0;
                }
            );
        }

        if (inItem.type == Chicane::FileSystem::ListType::File)
        {
            props.onClick = [&](const Chicane::Grid::ComponentEvent& inEvent)
            {
                Chicane::addActor(new MeshActor(inItem.path));

                return 0;
            };
            props._renderers.push_back(
                [&](const Chicane::Grid::ComponentEvent& inEvent)
                {
                    Chicane::Grid::Style style {};
                    style.horizontalAlignment = Chicane::Grid::Alignment::Center;
                    style.verticalAlignment   = Chicane::Grid::Alignment::Center;

                    Chicane::Grid::TextComponent::compileRaw(inItem.name, style);

                    return 0;
                }
            );
        }

        Chicane::Grid::ButtonComponent::compileRaw(props);
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

            if (!Chicane::Utils::areEquals(extension, ".box") && !Chicane::Utils::areEquals(extension, ".grid"))
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