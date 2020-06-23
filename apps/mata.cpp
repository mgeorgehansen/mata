#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <mata/lib.hpp>

int main() {
  try {
    const mata::App app;
    app.run();
  } catch (std::exception *error) {
    std::cout << "error: " << error->what() << std::endl;
    return 1;
  }
  return 0;
}
