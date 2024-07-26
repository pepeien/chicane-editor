#include "View.hpp"

namespace Factory
{
    View::View()
        : Chicane::Grid::View(
            "home",
            "Content/View/Home.grid"
        ),
        m_name(std::make_any<std::string>("")),
        m_imagePath(std::make_any<std::string>(""))
    {
        addVariable(
            "name",
            &m_name
        );
        addVariable(
            "imagePath",
            &m_imagePath
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
            "onSelectImage",
            std::bind(&View::onSelectImage, this, std::placeholders::_1)
        );
        addFunction(
            "onSubmit",
            std::bind(&View::onSubmit, this, std::placeholders::_1)
        );
    }

    std::uint32_t View::getFPS(Chicane::Grid::ComponentEvent inEvent)
    {
        return Chicane::State::getTelemetry().frame.rate;
    }

    std::string View::getFrametime(Chicane::Grid::ComponentEvent inEvent)
    {
        std::string frametime = std::to_string(Chicane::State::getTelemetry().frame.time);

        return std::string(frametime.begin(), frametime.end() - 5);
    }

    int View::onSelectImage(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::FileResult result = Chicane::FileSystem::openFileDialog(
            "Select Image",
            {
                { "PNG", "png" },
                { "TGA", "tga" }
            }
        );

        m_imagePath = result.path;

        return 0;
    }

    int View::onSubmit(Chicane::Grid::ComponentEvent inEvent)
    {
        Chicane::FileSystem::DirectoryResult result = Chicane::FileSystem::openDirectoryDialog();

        Chicane::Box::WriteInfo info = {};
        info.name       = std::any_cast<std::string>(m_name);
        info.outputPath = result.path;
        info.type       = Chicane::Box::Type::Texture;

        Chicane::Box::WriteEntry entry = {};
        entry.type = Chicane::Box::EntryType::Texture;
        entry.dataFilePath = std::any_cast<std::string>(m_imagePath);

        info.entries.push_back(entry);

        Chicane::Box::write(info);

        return 0;
    }
}