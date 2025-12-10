#include "Terminal.hpp"

#include <iostream>

using namespace stars;

std::pair<std::size_t, std::size_t> Terminal::getSize() {
    // TODO: calcular el ancho y algo de la terminal no es facil
    std::size_t fallbackWidth = 50;
    std::size_t fallbackHeight = 20;
    return {fallbackWidth, fallbackHeight};
}

void Terminal::write(const std::string& buffer) {
    std::cout << buffer << std::flush;
}

std::string Terminal::getHistoryPath() {
    // TODO: tambien hay que ver si es bash, u otro
    const char* home = std::getenv("HOME");
    if (!home) {
        throw std::runtime_error("HOME not set; provide --input path to history file.");
    }
    std::string path = std::string(home) + "/.bash_history";

    return path;
}