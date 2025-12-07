#pragma once
#include "graphics/Canvas.hpp"

namespace stars {

/// The ----
class Wire {
   public:
    virtual ~Wire();
    virtual void draw(Canvas& canvas) const = 0;
};

}  // namespace stars
