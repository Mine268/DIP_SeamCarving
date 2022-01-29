//
// Created by mine268 on 2022/1/29.
//

#pragma once

#include <string>
#include "RGBPixel.h"

using uchar = unsigned char;

/**
 * RGBImage class for image reading and writing.
 */
class RGBImage {
public:
    std::size_t height{0}, width{0}, channel{0}; // height & width of the image.
    RGBPixel *framebuffer; // Buffer that stored the image in RGB format.

    RGBImage() = delete;
    explicit RGBImage(std::size_t h, std::size_t w);
    explicit RGBImage(const std::string & path);
    ~RGBImage();

    /**
     * @brief Write the image to the path. PNG format.
     * @param path Path of the image.
     */
     virtual void write(const std::string & path);
};
