#include "UI/View/Home.hpp"

#include "Base.hpp"

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
            m_uiSelectedActor(std::make_any<std::string>("")),
            m_uiIsConsoleOpen(std::make_any<std::string>("false")),
            m_currentDirectory(""),
            m_selectedActor(nullptr)
        {
            m_uiActorTranslation.resize(3);
            m_uiActorTranslation[0] = std::make_any<std::string>("0.0");
            m_uiActorTranslation[1] = std::make_any<std::string>("0.0");
            m_uiActorTranslation[2] = std::make_any<std::string>("0.0");

            m_uiActorRotation.resize(3);
            m_uiActorRotation[0] = std::make_any<std::string>("0.0");
            m_uiActorRotation[1] = std::make_any<std::string>("0.0");
            m_uiActorRotation[2] = std::make_any<std::string>("0.0");

            m_uiActorScaling.resize(3);
            m_uiActorScaling[0] = std::make_any<std::string>("1.0");
            m_uiActorScaling[1] = std::make_any<std::string>("1.0");
            m_uiActorScaling[2] = std::make_any<std::string>("1.0");

            setupWatchers();

            setupUiTelemetry();
            setupUiActor();
            setupUiDirectory();
            setupUiAssetCreator();
            setupUiConsole();

            listDir(".");
        }

        std::uint32_t HomeView::getFPS(const Grid::Component::Event& inEvent)
        {
            return Application::getTelemetry().frame.rate;
        }

        std::string HomeView::getFrametime(const Grid::Component::Event& inEvent)
        {
            std::string frametime = std::to_string(Application::getTelemetry().frame.time);

            return std::string(frametime.begin(), frametime.end() - 5);
        }

        void HomeView::showLog(const Log::Entry& inLog)
        {
            if (inLog.isEmpty())
            {
                return;
            }

            Grid::Style style {};
            style.foregroundColor = inLog.color;

            Grid::TextComponent::compileRaw(inLog.text, style);
        }

        void HomeView::showActor(Actor* inActor)
        {
            if (!inActor)
            {
                return;
            }

            Grid::ButtonComponent::Props props {};
            props.id                    = Utils::sprint("%p", inActor);
            props.style.width           = Grid::getSize("100%");
            props.style.height          = Grid::getSize("4vh");
            props.style.backgroundColor = "#444444";
            props.onClick               = [&](const Grid::Component::Event& inEvent)
            {
                m_selectedActor = inActor;

                const Transform& transform = inActor->getTransform();

                m_uiActorTranslation[0] = std::make_any<std::string>(Utils::sprint("%.2f", transform.translation.x));
                m_uiActorTranslation[1] = std::make_any<std::string>(Utils::sprint("%.2f", transform.translation.y));
                m_uiActorTranslation[2] = std::make_any<std::string>(Utils::sprint("%.2f", transform.translation.z));

                m_uiActorRotation[0] = std::make_any<std::string>(Utils::sprint("%.2f", transform.rotation.x));
                m_uiActorRotation[1] = std::make_any<std::string>(Utils::sprint("%.2f", transform.rotation.y));
                m_uiActorRotation[2] = std::make_any<std::string>(Utils::sprint("%.2f", transform.rotation.z));

                m_uiActorScaling[0] = std::make_any<std::string>(Utils::sprint("%.2f", transform.scale.x));
                m_uiActorScaling[1] = std::make_any<std::string>(Utils::sprint("%.2f", transform.scale.y));
                m_uiActorScaling[2] = std::make_any<std::string>(Utils::sprint("%.2f", transform.scale.z));

                m_uiSelectedActor = std::make_any<std::string>(props.id);

                return 0;
            };
            props._renderers.push_back(
                [&](const Grid::Component::Event& inEvent)
                {
                    Grid::Style style {};
                    style.horizontalAlignment = Grid::Style::Alignment::Center;
                    style.verticalAlignment   = Grid::Style::Alignment::Center;

                    Grid::TextComponent::compileRaw(props.id, style);

                    return 0;
                }
            );

            Grid::ButtonComponent::compileRaw(props);
        }

        void HomeView::showDirectoryHistory(const std::string& inPath)
        {
            std::vector<std::string> splittedPath = Utils::split(
                inPath,
                FileSystem::SEPARATOR
            );

            const std::string& folderName = splittedPath.back();

            Grid::ButtonComponent::Props props {};
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

                return 0;
            };
            props._renderers.push_back(
                [&](const Grid::Component::Event& inEvent)
                {
                    Grid::Style style {};
                    style.horizontalAlignment = Grid::Style::Alignment::Center;
                    style.verticalAlignment   = Grid::Style::Alignment::Center;

                    Grid::TextComponent::compileRaw(folderName, style);

                    return 0;
                }
            );

            Grid::ButtonComponent::compileRaw(props);
        }

        void HomeView::showDirectory(const FileSystem::Item& inItem)
        {
            Grid::ButtonComponent::Props props {};
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

                    return 0;
                };
                props._renderers.push_back(
                    [&](const Grid::Component::Event& inEvent)
                    {
                        Grid::Style style {};
                        style.horizontalAlignment = Grid::Style::Alignment::Center;
                        style.verticalAlignment   = Grid::Style::Alignment::Center;

                        std::string title = inItem.name + " - " + std::to_string(inItem.childCount) + " Items";

                        Grid::TextComponent::compileRaw(title, style);

                        return 0;
                    }
                );
            }

            if (inItem.type == FileSystem::Item::Type::File)
            {
                if (inItem.extension == Box::Mesh::EXTENSION)
                {
                    props.onClick = [&](const Grid::Component::Event& inEvent)
                    {
                        Application::getLevel()->addActor(new MeshActor(inItem.path));

                        return 0;
                    };
                }

                props._renderers.push_back(
                    [&](const Grid::Component::Event& inEvent)
                    {
                        Grid::Style style {};
                        style.horizontalAlignment = Grid::Style::Alignment::Center;
                        style.verticalAlignment   = Grid::Style::Alignment::Center;

                        Grid::TextComponent::compileRaw(inItem.name, style);

                        return 0;
                    }
                );
            }

            Grid::ButtonComponent::compileRaw(props);
        }

        void HomeView::setupWatchers()
        {
            Log::watchLogs(
                [this](const Log::List& inLogs)
                {
                    std::vector<std::any> logs;

                    for (const Log::Entry& log : inLogs)
                    {
                        logs.push_back(std::make_any<Log::Entry>(log));
                    }

                    m_uiConsoleLogs = std::make_any<std::vector<std::any>>(logs);
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
                        [this](Actor* inActor)
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
            addVariable(
                "actors",
                &m_uiActors
            );
            addVariable(
                "selectedActor",
                &m_uiSelectedActor
            );
            addVariable(
                "actorTranslationX",
                &m_uiActorTranslation[0]
            );
            addVariable(
                "actorTranslationY",
                &m_uiActorTranslation[1]
            );
            addVariable(
                "actorTranslationZ",
                &m_uiActorTranslation[2]
            );
            addVariable(
                "actorRotationX",
                &m_uiActorRotation[0]
            );
            addVariable(
                "actorRotationY",
                &m_uiActorRotation[1]
            );
            addVariable(
                "actorRotationZ",
                &m_uiActorRotation[2]
            );
            addVariable(
                "actorScalingX",
                &m_uiActorScaling[0]
            );
            addVariable(
                "actorScalingY",
                &m_uiActorScaling[1]
            );
            addVariable(
                "actorScalingZ",
                &m_uiActorScaling[2]
            );

            // Functions
            addFunction(
                "showActor",
                [this](const Grid::Component::Event& inEvent)
                {
                    showActor(std::any_cast<Actor*>(inEvent.values[0]));

                    return 0;
                }
            );
            addFunction(
                "onTranslationChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateTranslation();

                    return 0;
                }
            );
            addFunction(
                "onRotationChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateRotation();

                    return 0;
                }
            );
            addFunction(
                "onScalingChange",
                [this](const Grid::Component::Event& inEvent)
                {
                    updateScaling();

                    return 0;
                }
            );
        }

        void HomeView::setupUiDirectory()
        {
            // Variables
            addVariable(
                "directoryHistory",
                &m_uiDirectoryHistory
            );
            addVariable(
                "directoryInfo",
                &m_uiDirectoryInfo
            );

            // Functions
            addFunction(
                "showDirectoryHistory",
                [this](const Grid::Component::Event& inEvent)
                {
                    showDirectoryHistory(std::any_cast<std::string>(inEvent.values[0]));

                    return 0;
                }
            );
            addFunction(
                "showDirectory",
                [this](const Grid::Component::Event& inEvent)
                {
                    showDirectory(std::any_cast<FileSystem::Item>(inEvent.values[0]));

                    return 0;
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

                                Box::Model model(filepath);
                                model.setId(name);
                                model.setVendor(Model::Vendor::Type::Wavefront);
                                model.setData(file.path.string());
                                model.saveXML();
                            }
                        }
                    );

                    return 0;
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

                                Box::Texture model(filepath);
                                model.setId(name);
                                model.setVendor(Texture::Vendor::Png);
                                model.setData(file.path.string());
                                model.saveXML();
                            }
                        }
                    );

                    return 0;
                }
            );
        }

        void HomeView::setupUiConsole()
        {
            // Variables
            addVariable(
                "consoleLogs",
                &m_uiConsoleLogs
            );
            addVariable(
                "isConsoleOpen",
                &m_uiIsConsoleOpen
            );

            // Functions
            addFunction(
                "showConsole",
                [this](const Grid::Component::Event& inEvent)
                {
                    m_uiIsConsoleOpen = std::make_any<std::string>("true");

                    return 0;
                }
            );
            addFunction(
                "hideConsole",
                [this](const Grid::Component::Event& inEvent)
                {
                    m_uiIsConsoleOpen = std::make_any<std::string>("false");

                    return 0;
                }
            );
            addFunction(
                "showLog",
                [this](const Grid::Component::Event& inEvent)
                {
                    showLog(std::any_cast<Log::Entry>(inEvent.values[0]));

                    return 0;
                }
            );
        }

        void HomeView::updateTranslation()
        {
            if (!m_selectedActor)
            {
                return;
            }

            std::string x = std::any_cast<std::string>(m_uiActorTranslation[0]);
            std::string y = std::any_cast<std::string>(m_uiActorTranslation[1]);
            std::string z = std::any_cast<std::string>(m_uiActorTranslation[2]);

            m_selectedActor->setAbsoluteTranslation(
                Vec<3, float>(
                    x.empty() ? 0.0f : std::stof(x),
                    y.empty() ? 0.0f : std::stof(y),
                    z.empty() ? 0.0f : std::stof(z)
                )
            );
        }

        void HomeView::updateRotation()
        {
            if (!m_selectedActor)
            {
                return;
            }

            std::string x = std::any_cast<std::string>(m_uiActorRotation[0]);
            std::string y = std::any_cast<std::string>(m_uiActorRotation[1]);
            std::string z = std::any_cast<std::string>(m_uiActorRotation[2]);

            m_selectedActor->setAbsoluteRotation(
                Vec<3, float>(
                    x.empty() ? 0.0f : std::stof(x),
                    y.empty() ? 0.0f : std::stof(y),
                    z.empty() ? 0.0f : std::stof(z)
                )
            );
        }

        void HomeView::updateScaling()
        {
            if (!m_selectedActor)
            {
                return;
            }

            std::string x = std::any_cast<std::string>(m_uiActorScaling[0]);
            std::string y = std::any_cast<std::string>(m_uiActorScaling[1]);
            std::string z = std::any_cast<std::string>(m_uiActorScaling[2]);

            m_selectedActor->setAbsoluteScale(
                Vec<3, float>(
                    x.empty() ? 1.0f : std::stof(x),
                    y.empty() ? 1.0f : std::stof(y),
                    z.empty() ? 1.0f : std::stof(z)
                )
            );
        }

        void HomeView::updateOutliner()
        {
            std::unordered_map<std::string, std::uint32_t> ids {};

            std::vector<std::any> actors {};

            for (Actor* actor : Application::getLevel()->getActors())
            {
                actors.push_back(std::make_any<Actor*>(actor));
            }

            m_uiActors = std::make_any<std::vector<std::any>>(actors);
        }

        void HomeView::updateDirHistory()
        {
            std::vector<std::any> items = {};

            std::vector<std::string> splittedPath = Utils::split(
                m_currentDirectory,
                FileSystem::SEPARATOR
            );

            for (std::uint32_t i = 0; i < splittedPath.size(); i++)
            {
                std::string path = "";

                for (std::uint32_t j = 0; j <= i; j++)
                {
                    path += splittedPath.at(j);
                    path += FileSystem::SEPARATOR;
                }

                items.push_back(
                    std::make_any<std::string>(path)
                );
            }

            m_uiDirectoryHistory = std::make_any<std::vector<std::any>>(items);
        }

        void HomeView::listDir(const std::string& inPath)
        {
            std::vector<std::any> items = {};

            for (const auto& item : FileSystem::ls(inPath))
            {
                if (item.type == FileSystem::Item::Type::Folder)
                {
                    items.push_back(
                        std::make_any<FileSystem::Item>(item)
                    );

                    continue;
                }

                std::vector<std::string> extensions {
                    Box::CubeMap::EXTENSION,
                    Box::Mesh::EXTENSION,
                    Box::Model::EXTENSION,
                    Box::Texture::EXTENSION,
                    ".grid"
                };

                if (std::find(extensions.begin(), extensions.end(), item.extension) == extensions.end())
                {
                    continue;
                }

                items.push_back(
                    std::make_any<FileSystem::Item>(item)
                );
            }

            m_uiDirectoryInfo = std::make_any<std::vector<std::any>>(items);

            m_currentDirectory = inPath;

            updateDirHistory();
        }
    }
}