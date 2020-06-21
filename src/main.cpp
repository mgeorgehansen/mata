#include <exception>
#include <iostream>
#include <string>
#include <memory>
#include <filesystem>

#include "app.h"

int main()
{
    try
    {
        const App app;
        app.run();
    }
    catch (std::exception *error)
    {
        std::cout << "error: " << error->what() << std::endl;
        return 1;
    }
    return 0;
}
