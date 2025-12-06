#pragma once
#include <string>
#include <vector>

namespace stars {

class Canvas {
public:
  Canvas(int w, int h, char blank = ' ');
  int width() const;
  int height() const;

  bool inBounds(int x, int y) const;
  bool put(int x, int y, char c);

  bool putString(int x, int y, const std::string& s, bool nudgeDown = true, int maxNudges = 2);

  std::string render() const;

private:
  int w_;
  int h_;
  char blank_;
  std::vector<std::string> buf_;
};

} // namespace stars
