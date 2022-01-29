//
// Created by mine268 on 2022/1/29.
//

#pragma once

#include <cstddef>

struct RGBPixel {
    unsigned char red{0}, green{0}, blue{0}, alpha{0};

    /**
     * @brief Return the color in grey form correspondingly.
     * @return The grey value for this color, range in [0,1].
     */
    float grey() const {
        return (0.299f * red + 0.587f * green + 0.114f * blue) / 255.f;
    }
};

struct PixelPos {
    std::size_t i, j;
};
