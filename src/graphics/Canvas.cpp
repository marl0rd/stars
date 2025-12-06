#include "graphics/Canvas.hpp"
#include <algorithm>

namespace stars {

Canvas::Canvas(int w, int h, char blank)
  : w_(w), h_(h), blank_(blank), buf_(h, std::string(w, blank)) {}

int Canvas::width() const { return w_; }
int Canvas::height() const { return h_; }

bool Canvas::inBounds(int x, int y) const {
  return x >= 0 && y >= 0 && x < w_ && y < h_;
}

bool Canvas::put(int x, int y, char c) {
  if (!inBounds(x, y)) return false;
  if (buf_[y][x] != blank_) return false;
  buf_[y][x] = c;
  return true;
}

bool Canvas::putString(int x, int y, const std::string& s, bool nudgeDown, int maxNudges) {
  int ny = y;
  for (int attempt = 0; attempt <= maxNudges; ++attempt) {
    if (x + (int)s.size() > w_) return false;
    bool ok = true;
    for (int i = 0; i < (int)s.size(); ++i) {
      if (!inBounds(x + i, ny) || buf_[ny][x + i] != blank_) { ok = false; break; }
    }
    if (ok) {
      for (int i = 0; i < (int)s.size(); ++i) buf_[ny][x + i] = s[i];
      return true;
    }
    if (!nudgeDown) break;
    ny++; if (ny >= h_) break;
  }
  return false;
}

std::string Canvas::render() const {
  std::string out;
  out.reserve(h_ * w_);
  for (const auto& row : buf_) {
    size_t end = row.find_last_not_of(blank_);
    out.append(row.substr(0, (end == std::string::npos) ? 0 : end + 1));
    out.push_back('\n');
  }
  return out;
}

} // namespace stars
