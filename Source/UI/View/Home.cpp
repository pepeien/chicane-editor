#include "UI/View/Home.hpp"

#include "Actor/Mesh.hpp"

constexpr const SDL_DialogFileFilter m_modelFilters[] = {
    { "All (.obj)", "obj" },
    { "Wavefront (.obj)", "obj" },
    { nullptr, nullptr }
};

constexpr const SDL_DialogFileFilter m_textureFilters[] = {
    { "All (.png)", "png" },
    { "Portable Network Graphics (.png)", "png" },
    { nullptr, nullptr }
};

namespace Chicane
{
    namespace Editor
    {
        HomeView::HomeView()
            : Grid::View(
                "editor_home",
                "Content/Views/Home.grid"
            ),
            m_directoryHistory({}),
            m_directoryHistoryRefs({}),
            m_directoryInfo({}),
            m_directoryInfoRefs({}),
            m_actors({}),
            m_actor(nullptr),
            m_bIsConsoleOpen(false),
            m_consoleLogs({}),
            m_uiDirectoryHistory(Grid::Reference::fromValue<std::vector<Grid::Reference>>(&m_directoryHistoryRefs)),
            m_uiDirectoryInfo(Grid::Reference::fromValue<std::vector<Grid::Reference>>(&m_directoryInfoRefs)),
            m_uiActors({}),
            m_uiIsConsoleOpen(Grid::Reference::fromValue<bool>(&m_bIsConsoleOpen)),
            m_uiConsoleLogs(Grid::Reference::fromValue<std::vector<Grid::Reference>>(&m_consoleLogs))
        {
            setupWatchers();

            setupUiTelemetry();
            setupUiActor();
            setupUiDirectory();
            setupUiAssetCreator();
            setupUiConsole();

            listDir(".");
        }

        Grid::Reference HomeView::getFPS(const Grid::Component::Event& inEvent)
        {
            return Grid::Reference::fromValue<const std::uint32_t>(&Application::getTelemetry().frame.rate);
        }

        Grid::Reference HomeView::getFrametime(const Grid::Component::Event& inEvent)
        {
            return Grid::Reference::fromValue<const float>(&Application::getTelemetry().frame.delta);
        }

        void HomeView::showLog(const Log::Entry* inLog)
        {
            if (inLog->isEmpty())
            {
                return;
            }

            Grid::Style style {};
            style.foregroundColor = inLog->color;

            Grid::Text::compileRaw(inLog->text, style);
        }

        void HomeView::showActor(const Actor* inActor)
        {
            if (!inActor)
            {
                return;
            }

            Grid::Button::Props props {};
            props.id                    = String::sprint("%p", inActor);
            props.style.width           = Grid::getSize("100%");
            props.style.height          = Grid::getSize("4vh");
            props.style.backgroundColor = "#444444";
            props.onClick               = [&](const Grid::Component::Event& inEvent)
            {
                m_actor = inActor;
            };
            props._renderers.push_back(
                [&](const Grid::Component::Event& inEvent)
                {
                    Grid::Style style {};
                    style.horizontalAlignment = Grid::Style::Alignment::Center;
                    style.verticalAlignment   = Grid::Style::Alignment::Center;

                    Grid::Text::compileRaw(props.id, style);
                }
            );

            Grid::Button::compileRaw(props);
        }

        void HomeView::showDirectoryHistory(const std::string& inPath)
        {
            std::vector<std::string> splittedPath = String::split(
                inPath,
                FileSystem::SEPARATOR
            );

            const std::string& folderName = splittedPath.back();

            Grid::Button::Props props {};
            props.id = inPath;
            props.style.width = Grid::getSize(
                "5vw",
                Grid::Style::Direction::Horizontal,
                Grid::Style::Position::Relative
            );
            props.style.height = Grid::getSize(
                Grid::AUTO_SIZE_UNIT,
                Grid::Style::Direction::Vertical,
                Grid::Style::Position::Relative
            );
            props.style.backgroundColor = "#444444";
            props.onClick = [&](const Grid::Component::Event& inEvent)
            {
                listDir(inPath);
            };
            props._renderers.push_back(
                [&](const Grid::Component::Event& inEvent)
                {
                    Grid::Style style {};
                    style.horizontalAlignment = Grid::Style::Alignment::Center;
                    style.verticalAlignment   = Grid::Style::Alignment::Center;

                    Grid::Text::compileRaw(folderName, style);
                }
            );

            Grid::Button::compileRaw(props);
        }

        void HomeView::showDirectory(const FileSystem::Item& inItem)
        {
            Grid::Button::Props props {};
            props.id = inItem.path;
            props.style.width = Grid::getSize(
                Grid::AUTO_SIZE_UNIT,
                Grid::Style::Direction::Horizontal,
                Grid::Style::Position::Relative
            );
            props.style.height = Grid::getSize(
                Grid::AUTO_SIZE_UNIT,
                Grid::Style::Direction::Vertical,
                Grid::Style::Position::Relative
            );
            props.style.backgroundColor = "#444444";

            if (inItem.type == FileSystem::Item::Type::Folder)
            {
                props.onClick = [&](const Grid::Component::Event& inEvent)
                {
                    listDir(inItem.path);
                };

                props._renderers.push_back(
                    [&](const Grid::Component::Event& inEvent)
                    {
                        Grid::Style style {};
                        style.horizontalAlignment = Grid::Style::Alignment::Center;
                        style.verticalAlignment   = Grid::Style::Alignment::Center;

                        std::string title = inItem.name + " - " + std::to_string(inItem.childCount) + " Items";

                        Grid::Text::compileRaw(title, style);
                    }
                );
            }

            if (inItem.type == FileSystem::Item::Type::File)
            {
                if (inItem.extension == Box::Mesh::EXTENSION)
                {
                    props.onClick = [&](const Grid::Component::Event& inEvent)
                    {
                        Application::getLevel()->createActor<AEditorMesh>()->setMesh(inItem.path);
                    };
                }

                props._renderers.push_back(
                    [&](const Grid::Component::Event& inEvent)
                    {
                        Grid::Style style {};
                        style.horizontalAlignment = Grid::Style::Alignment::Center;
                        style.verticalAlignment   = Grid::Style::Alignment::Center;

                        Grid::Text::compileRaw(inItem.name, style);
                    }
                );
            }

            Grid::Button::compileRaw(props);
        }

        void HomeView::setupWatchers()
        {
            Log::watchLogs(
                [this](const Log::List* inLogs)
                {
                    m_consoleLogs = {};

                    for (std::uint32_t i = 0; i < inLogs->size(); i++)
                    {
                        m_consoleLogs.push_back(
                            Grid::Reference::fromValue<const Log::Entry>(
                                &inLogs->at(i)
                            )
                        );
                    }
                }
            );

            Application::watchLevel(
                [this](Level* inLevel)
                {
                    if (!inLevel)
                    {
                        updateOutliner();
                    }

                    inLevel->watchActors(
                        [this](const std::vector<Actor*>& inActors)
                        {
                            updateOutliner();
                        }
                    );
                }
            );
        }

        void HomeView::setupUiTelemetry()
        {
            // Functions
            addFunction(
                "getFPS",
                std::bind(&HomeView::getFPS, this, std::placeholders::_1)
            );
            addFunction(
                "getFrametime",
                std::bind(&HomeView::getFrametime, this, std::placeholders::_1)
            );
        }

        void HomeView::setupUiActor()
        {
            // Variables
            addVariable("actors", &m_uiActors);

            // Functions
            addFunction(
                "showActor",
                [this](const Grid::Component::Event& inEvent)
                {
                    showActor(inEvent.values[0].getValue<Actor>());

                    return Grid::Reference();
                }
            );
            addFunction(
                "onTranslationChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateTranslation();

                    return Grid::Reference();
                }
            );
            addFunction(
                "onRotationChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateRotation();

                    return Grid::Reference();
                }
            );
            addFunction(
                "onScalingChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateScaling();

                    return Grid::Reference();
                }
            );
        }

        void HomeView::setupUiDirectory()
        {
            // Variables
            addVariable("directoryHistory", &m_uiDirectoryHistory);
            addVariable("directoryInfo", &m_uiDirectoryInfo);

            // Functions
            addFunction(
                "showDirectoryHistory",
                [this](const Grid::Component::Event& inEvent)
                {
                    for (const Grid::Reference& reference : inEvent.values)
                    {
                        if (reference.isEmpty() || !reference.isType<const std::string>())
                        {
                            continue;
                        }

                        showDirectoryHistory(*reference.getValue<const std::string>());
                    }

                    return Grid::Reference();
                }
            );
            addFunction(
                "showDirectory",
                [this](const Grid::Component::Event& inEvent)
                {
                    for (const Grid::Reference& reference : inEvent.values)
                    {
                        if (reference.isEmpty() || !reference.isType<const FileSystem::Item>())
                        {
                            continue;
                        }

                        showDirectory(*reference.getValue<const FileSystem::Item>());
                    }

                    return Grid::Reference();
                }
            );
        }

        void HomeView::setupUiAssetCreator()
        {
            // Functions
            addFunction(
                "showModelCreator",
                [this](const Grid::Component::Event& inEvent)
                {
                    FileSystem::FileDialog props {};
                    props.title         = "Select 3D Model";
                    props.filters       = m_modelFilters;
                    props.filterCount   = 2;
                    props.canSelectMany = false;

                    FileSystem::openFileDialog(
                        props,
                        [](void* inData, const char* const* inFiles, int inFilter)
                        {
                            const auto& result = FileSystem::Dialog::Result::fromRaw(inFiles);

                            if (result.empty())
                            {
                                return;
                            }

                            for (const auto& file : result)
                            {
                                std::string path = file.path.parent_path().string();
                                std::string name = file.path.stem().string();
                                std::string filepath = path;
                                filepath += (char) FileSystem::SEPARATOR;
                                filepath += name;
                                filepath += Box::Model::EXTENSION;

                                Box::Model::Instance model(filepath);
                                model.setId(name);
                                model.setVendor(Box::Model::Vendor::Type::Wavefront);
                                model.setData(file.path.string());
                                model.saveXML();
                            }
                        }
                    );

                    return Grid::Reference();
                }
            );
            addFunction(
                "showTextureCreator",
                [this](const Grid::Component::Event& inEvent)
                {
                    FileSystem::FileDialog props {};
                    props.title         = "Select a Texture Image";
                    props.filters       = m_textureFilters;
                    props.filterCount   = 2;
                    props.canSelectMany = false;

                    FileSystem::openFileDialog(
                        props,
                        [](void* inData, const char* const* inFiles, int inFilter)
                        {
                            const auto& result = FileSystem::Dialog::Result::fromRaw(inFiles);

                            if (result.empty())
                            {
                                return;
                            }

                            for (const auto& file : result)
                            {
                                std::string path = file.path.parent_path().string();
                                std::string name = file.path.stem().string();
                                std::string filepath = path;
                                filepath += (char) FileSystem::SEPARATOR;
                                filepath += name;
                                filepath += Box::Texture::EXTENSION;

                                Box::Texture::Instance texture(filepath);
                                texture.setId(name);
                                texture.setVendor(Box::Texture::Vendor::Png);
                                texture.setData(file.path.string());
                                texture.saveXML();
                            }
                        }
                    );

                    return Grid::Reference();
                }
            );
        }

        void HomeView::setupUiConsole()
        {
            // Variables
            addVariable("consoleLogs", &m_uiConsoleLogs);
            addVariable("isConsoleOpen", &m_uiIsConsoleOpen);

            // Functions
            addFunction(
                "showConsole",
                [this](const Grid::Component::Event& inEvent)
                {
                    m_bIsConsoleOpen = true;

                    return Grid::Reference();
                }
            );
            addFunction(
                "hideConsole",
                [this](const Grid::Component::Event& inEvent)
                {
                    m_bIsConsoleOpen = false;

                    return Grid::Reference();
                }
            );
            addFunction(
                "showLog",
                [this](const Grid::Component::Event& inEvent)
                {
                    for (const Grid::Reference& reference : inEvent.values)
                    {
                        if (reference.isEmpty() || !reference.isType<const Log::Entry>())
                        {
                            continue;
                        }

                        showLog(reference.getValue<const Log::Entry>());
                    }

                    return Grid::Reference();
                }
            );
        }

        void HomeView::updateTranslation()
        {
            if (!m_actor)
            {
                return;
            }
        }

        void HomeView::updateRotation()
        {
            if (!m_actor)
            {
                return;
            }
        }

        void HomeView::updateScaling()
        {
            if (!m_actor)
            {
                return;
            }
        }

        void HomeView::updateOutliner()
        {
            m_actors = Application::getLevel()->getActors();
        }

        void HomeView::updateDirHistory(const std::string& inPath)
        {
            m_directoryHistory     = {};
            m_directoryHistoryRefs = {};

            std::vector<std::string> splittedPath = String::split(inPath, FileSystem::SEPARATOR);

            for (std::uint32_t i = 0; i < splittedPath.size(); i++)
            {
                std::string path = "";

                for (std::uint32_t j = 0; j <= i; j++)
                {
                    path += splittedPath.at(j);
                    path += FileSystem::SEPARATOR;
                }

                m_directoryHistory.push_back(path);
            }

            for (std::uint32_t i = 0; i < m_directoryHistory.size(); i++)
            {
                m_directoryHistoryRefs.push_back(
                    Grid::Reference::fromValue<const std::string>(
                        &m_directoryHistory.at(i)
                    )
                );
            }
        }

        void HomeView::listDir(const std::string& inPath)
        {
            m_directoryInfo     = {};
            m_directoryInfoRefs = {};

            std::vector<std::string> extensions {
                Box::Sky::EXTENSION,
                Box::Mesh::EXTENSION,
                Box::Model::EXTENSION,
                Box::Texture::EXTENSION,
                ".grid"
            };

            for (FileSystem::Item item : FileSystem::ls(inPath))
            {
                if (item.type == FileSystem::Item::Type::Folder)
                {
                    m_directoryInfo.push_back(item);

                    continue;
                }

                if (std::find(extensions.begin(), extensions.end(), item.extension) == extensions.end())
                {
                    continue;
                }

                m_directoryInfo.push_back(item);
            }

            for (std::uint32_t i = 0; i < m_directoryInfo.size(); i++)
            {
                m_directoryInfoRefs.push_back(
                    Grid::Reference::fromValue<const FileSystem::Item>(
                        &m_directoryInfo.at(i)
                    )
                );
            }

            updateDirHistory(inPath);
        }
    }
}