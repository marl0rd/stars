#pragma once
#include <string>

#include "graphics/Canvas.hpp"

namespace stars {

/// A star like: "* <label>".
class Star {
   public:
    explicit Star(std::string label);

    int getDrawWidth() const;
    void draw(Canvas& canvas, int x, int y) const;

   private:
    std::string label_;
};

}  // namespace stars
