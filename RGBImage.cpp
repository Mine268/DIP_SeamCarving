//
// Created by mine268 on 2022/1/29.
//

#include "RGBImage.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

RGBImage::RGBImage(std::size_t h, std::size_t w) :
height(h), width(w), channel(0), framebuffer(new RGBPixel[h * w]) {}

RGBImage::RGBImage(const std::string & path) {
    int w, h, t_channel;
    auto raw_data = stbi_load(path.c_str(), &w, &h, &t_channel, 0);
    if (raw_data == nullptr)
        throw std::string("Unable to open the image.");
    height = h;
    width = w;
    channel = static_cast<std::size_t>(t_channel);
    framebuffer = new RGBPixel[h * w];
    for (std::size_t i = 0; i < h * w; ++i) {
        framebuffer[i].red = raw_data[channel * i + 0];
        framebuffer[i].green = raw_data[channel * i + 1];
        framebuffer[i].blue = raw_data[channel * i + 2];
        framebuffer[i].alpha = raw_data[channel * i + 3];
    }
    stbi_image_free(raw_data);
}

RGBImage::~RGBImage() {
    delete[] framebuffer;
}

void RGBImage::write(const std::string &path) {
    auto * raw_data = new uchar[width * height * channel];
    for (std::size_t i = 0; i < width * height; ++i) {
        raw_data[channel * i + 0] = framebuffer[i].red;
        raw_data[channel * i + 1] = framebuffer[i].green;
        raw_data[channel * i + 2] = framebuffer[i].blue;
        raw_data[channel * i + 3] = framebuffer[i].alpha;
    }
    stbi_write_png(path.c_str(),
                   static_cast<int>(width),
                   static_cast<int>(height),
                   static_cast<int>(channel),
                   raw_data, 0);
    delete[] raw_data;
}
