#include "Main.hpp"

#include "Editor.hpp"

int main(int argc, char *argv[])
{
    try
    {
        Factory::run();
    }
    catch (const std::exception& e)
    {
        Chicane::Log::critical(e.what());

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}