#include <exception>
#include <iostream>
#include <string>

#include "renderer.h"

int main()
{
    try
    {
        Renderer renderer;
        renderer.run();
    }
    catch (std::exception *error)
    {
        std::cout << "error: " << error->what() << std::endl;
        return 1;
    }
    return 0;
}
