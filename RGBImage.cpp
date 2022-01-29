//
// Created by mine268 on 2022/1/29.
//

#include "RGBImage.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <iostream>
#include <cfloat>

RGBImage::RGBImage(std::size_t h, std::size_t w) :
height(h), width(w), channel(0), framebuffer(new RGBPixel[h * w]), energyMap(new ST_TWO_FLOAT[h * w]) {}

RGBImage::RGBImage(const std::string & path) {
    int w, h, t_channel;
    auto raw_data = stbi_load(path.c_str(), &w, &h, &t_channel, 0);
    if (raw_data == nullptr) {
        std::cerr << "Unable to open the image." << std::endl;
        return;
    }
    height = h;
    width = w;
    channel = static_cast<std::size_t>(t_channel);
    framebuffer = new RGBPixel[h * w];
    energyMap = new ST_TWO_FLOAT[h * w];
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
    delete[] energyMap;
    framebuffer = nullptr;
    energyMap = nullptr;
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

ST_TWO_FLOAT & RGBImage::atEnergyMap(std::size_t i, std::size_t j) {
    return energyMap[i * width + j];
}

RGBPixel & RGBImage::at(std::size_t i, std::size_t j) {
    return framebuffer[i * width + j];
}

float RGBImage::evaluateEnergyAt(std::size_t i, std::size_t j) {
    // Energy form "e1" is implemented here.
    // Heuristically, we use grey channel to evaluate the energy
    auto curr = at(i, j).grey();
    auto deltaI = (i == height - 1 ? at(0, j) : at(i + 1, j)).grey();
    auto deltaJ = (j == width - 1 ? at(i, 0) : at(i, j + 1)).grey();
    return std::abs(deltaI - curr) + std::abs(deltaJ - curr);
}

Seam_Path RGBImage::combVertical() {
    // DP: Initialize the starting state
    for (std::size_t j = 0; j < width; ++j) {
        atEnergyMap(0, j).ver = evaluateEnergyAt(0, j);
    }
    // DP: Iterate through the energy map
    for (std::size_t i = 1; i < height; ++i) {
        float tmp_min = FLT_MAX;
        std::size_t tmp_prev = -1;
        for (std::size_t j = 0; j < width; ++j) {
            for (int d = -1; d <= 1; ++d) {
                if (((int) j) + d >= 0 && ((int) j) + d < width
                    && tmp_min > evaluateEnergyAt(i, j) + atEnergyMap(i - 1, j + d).ver) {
                    tmp_min = evaluateEnergyAt(i, j) + atEnergyMap(i - 1, j + d).ver;
                    tmp_prev = j + d;
                }
            }
            atEnergyMap(i, j).ver = tmp_min;
            atEnergyMap(i, j).prev_j = tmp_prev;
        }
    }
    // DP: Backwards search to construct the optimal path
    float goal_path_energy = FLT_MAX;
    std::size_t goal_path_j = -1;
    Seam_Path ret;
    for (std::size_t j = 0; j < width; ++j) {
        if (atEnergyMap(height - 1, j).ver < goal_path_energy) {
            goal_path_energy = atEnergyMap(height - 1, j).ver;
            goal_path_j = j;
        }
    }
    ret.energy = goal_path_energy;
    for (std::size_t i = height - 1; i >= 0; --i) {
        ret.path.push_back({i, goal_path_j});
        goal_path_j = atEnergyMap(i, goal_path_j).prev_j;
    }
    return ret;
}

Seam_Path RGBImage::combHorizontal() {
    for (std::size_t i = 0; i < height; ++i) {
        atEnergyMap(i, 0).ver = evaluateEnergyAt(i, 0);
    }
}
