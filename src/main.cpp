#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "app.h"

int main() {
  try {
    const App app;
    app.run();
  } catch (std::exception *error) {
    std::cout << "error: " << error->what() << std::endl;
    return 1;
  }
  return 0;
}
