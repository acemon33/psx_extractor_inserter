#pragma once

#include <stdexcept>
#include <string>
#include <iostream>


namespace Utilities
{
    inline void handle_error(const std::string& message) {
        std::cerr << "Error: " << message << std::endl;
        throw std::runtime_error(message);
    }
}
