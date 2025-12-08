#include "graphics/Canvas.hpp"

#include <algorithm>
#include <stdexcept>

using namespace stars;

Canvas::Canvas(int w, int h)
    : w_(w), h_(h), buffer_(h, std::string(w, BLANK_CHAR)) {}

int Canvas::width() const { return w_; }
int Canvas::height() const { return h_; }

bool Canvas::isInBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < w_ && y < h_;
}

void Canvas::putChar(int x, int y, char c) {
    if (!isInBounds(x, y)) throw std::out_of_range("Coordinates out of bounds");
    if (buffer_[y][x] != BLANK_CHAR) throw std::runtime_error("Position already occupied");
    buffer_[y][x] = c;
}

void Canvas::putString(int x, int y, const std::string& s, bool shiftDown, int maxshifts) {
    bool ok = true;
    for (int attempt = 0; attempt <= maxshifts; ++attempt) {
        if (x + (int)s.size() > w_) {
            throw std::out_of_range("String exceeds canvas width");
        }

        // Check for collisions
        for (int i = 0; i < (int)s.size(); ++i) {
            if (!isInBounds(x + i, y) || buffer_[y][x + i] != BLANK_CHAR) {
                throw std::runtime_error("Position already occupied");
            }
        }

        if (ok) {
            for (int i = 0; i < (int)s.size(); ++i) {
                buffer_[y][x + i] = s[i];
            }
            return;
        }

        if (!shiftDown) break;
        y++;

        if (y >= h_) break;
    }
}

std::string Canvas::getRender() const {
    std::string out;
    out.reserve(h_ * w_);
    for (const auto& row : buffer_) {
        size_t end = row.find_last_not_of(BLANK_CHAR);
        out.append(row.substr(0, (end == std::string::npos) ? 0 : end + 1));
        out.push_back('\n');
    }
    return out;
}
