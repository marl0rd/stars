#pragma once
#include <string>
#include <vector>

namespace stars {

class Canvas {
   public:
    Canvas(int w, int h);
    int width() const;
    int height() const;

    bool isInBounds(int x, int y) const;
    void putChar(int x, int y, char c);
    void putString(int x, int y, const std::string& s, bool shiftDown = true, int maxShifts = 2);

    std::string getRender() const;

   private:
    int w_;
    int h_;
    static constexpr char BLANK_CHAR = ' ';
    std::vector<std::string> buffer_;
};

}  // namespace stars
