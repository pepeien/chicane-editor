#include "View.hpp"

namespace Factory
{
    View::View()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        ),
        m_cubemapFileFormat({}),
        m_cubemapInfo({}),
        m_name(std::make_any<std::string>("")),
        m_frontImage(std::make_any<std::string>("")),
        m_behindImage(std::make_any<std::string>("")),
        m_leftImage(std::make_any<std::string>("")),
        m_rightImage(std::make_any<std::string>("")),
        m_upImage(std::make_any<std::string>("")),
        m_downImage(std::make_any<std::string>(""))
    {
        m_cubemapFileFormat.title     = "TGA";
        m_cubemapFileFormat.extension = "tga";

        m_cubemapInfo.type = Chicane::Box::Type::CubeMap;
        m_cubemapInfo.entries.resize(6);

        addFunction(
            "getFPS",
            std::bind(&View::getFPS, this, std::placeholders::_1)
        );
        addFunction(
            "getFrametime",
            std::bind(&View::getFrametime, this, std::placeholders::_1)
        );

        // Form
        addFunction(
            "addFrontImage",
            std::bind(&View::addFrontImage, this, std::placeholders::_1)
        );
        addFunction(
            "addBehindImage",
            std::bind(&View::addBehindImage, this, std::placeholders::_1)
        );
        addFunction(
            "addLeftImage",
            std::bind(&View::addLeftImage, this, std::placeholders::_1)
        );
        addFunction(
            "addRightImage",
            std::bind(&View::addRightImage, this, std::placeholders::_1)
        );
        addFunction(
            "addUpImage",
            std::bind(&View::addUpImage, this, std::placeholders::_1)
        );
        addFunction(
            "addDownImage",
            std::bind(&View::addDownImage, this, std::placeholders::_1)
        );
        addFunction(
            "onSubmit",
            std::bind(&View::onSubmit, this, std::placeholders::_1)
        );

        addVariable(
            "name",
            &m_name
        );
        addVariable(
            "frontImage",
            &m_frontImage
        );
        addVariable(
            "behindImage",
            &m_behindImage
        );
        addVariable(
            "leftImage",
            &m_leftImage
        );
        addVariable(
            "rightImage",
            &m_rightImage
        );
        addVariable(
            "upImage",
            &m_upImage
        );
        addVariable(
            "downImage",
            &m_downImage
        );
    }

    std::uint64_t View::getFPS(Chicane::Grid::ComponentEvent inEvent)
    {
        return Chicane::State::getTelemetry().framerate;
    }

    std::string View::getFrametime(Chicane::Grid::ComponentEvent inEvent)
    {
        std::string frametime = std::to_string(Chicane::State::getTelemetry().time);

        return std::string(frametime.begin(), frametime.end() - 5);
    }

    int View::addFrontImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Front Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_frontImage = result.path;

        m_cubemapInfo.entries[0] = entry;

        return 0; 
    }

    int View::addBehindImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Behind Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_behindImage = result.path;

        m_cubemapInfo.entries[1] = entry;

        return 0;
    }

    int View::addLeftImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Left Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_leftImage = result.path;

        m_cubemapInfo.entries[2] = entry;

        return 0;
    }

    int View::addRightImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Right Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_rightImage = result.path;

        m_cubemapInfo.entries[3] = entry;

        return 0;
    }

    int View::addUpImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Up Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_upImage = result.path;

        m_cubemapInfo.entries[4] = entry;

        return 0;
    }

    int View::addDownImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Choose Down Image",
            { m_cubemapFileFormat }
        );

        Chicane::Box::WriteEntry entry = {};
        entry.type     = Chicane::Box::EntryType::Texture;
        entry.filePath = result.path;

        m_downImage = result.path;

        m_cubemapInfo.entries[5] = entry;

        return 0;
    }

    int View::onSubmit(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::DirectoryResult result = Chicane::FileSystem::openDirectoryDialog();

        m_cubemapInfo.name       = std::any_cast<std::string>(m_name);
        m_cubemapInfo.type       = Chicane::Box::Type::CubeMap;
        m_cubemapInfo.outputPath = result.path;

        Chicane::Box::write(
            m_cubemapInfo
        );

        return 0;
    }
}