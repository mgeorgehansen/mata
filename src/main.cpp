#include <exception>
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>

#include "renderer.h"
#include "virtual-file-system.h"
#include "utils/filesystem.h"

int main()
{
    try
    {
        auto vfs = std::make_shared<VirtualFileSystem>(
            execDir() / "resources");
        Renderer renderer(vfs);
        renderer.run();
    }
    catch (std::exception *error)
    {
        std::cout << "error: " << error->what() << std::endl;
        return 1;
    }
    return 0;
}
