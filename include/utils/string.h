#include <memory>
#include <string>
#include <exception>

template <class... Args>
std::string format(const std::string &fmt, const Args &... args)
{
    const size_t size = std::snprintf(nullptr, 0, fmt.c_str(), args...) + 1;
    if (size < 0)
    {
        throw new std::runtime_error("encoding error during formatting");
    }
    std::unique_ptr<char[]> buffer(new char[size]);
    snprintf(buffer.get(), size, fmt.c_str(), args...);
    return std::string(buffer.get(), buffer.get() + size - 1);
}
