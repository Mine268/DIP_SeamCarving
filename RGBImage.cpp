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
#include <algorithm>
#include <numeric>

RGBImage::RGBImage(std::size_t h, std::size_t w) :
        height(h), width(w), channel(0), framebuffer(new RGBPixel[h * w]), energyMap(new ST_TWO_FLOAT[h * w]) {}

RGBImage::RGBImage(const std::string &path) {
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

RGBImage::RGBImage(const RGBImage &origin) :
        height(origin.height), width(origin.width), channel(origin.channel),
        framebuffer(new RGBPixel[origin.height * origin.width]),
        energyMap(new ST_TWO_FLOAT[origin.height * origin.width]),
        concentration(origin.concentration) {
    for (std::size_t i = 0; i < height * width; ++i) {
        framebuffer[i] = origin.framebuffer[i];
    }
}

RGBImage &RGBImage::operator=(const RGBImage &origin) {
    // self-assignment process
    if (this == &origin) return *this;
    if (height != origin.height || width != origin.width) {
        delete[] framebuffer;
        delete[] energyMap;
        framebuffer = new RGBPixel[origin.height * origin.width];
        energyMap = new ST_TWO_FLOAT[origin.height * origin.width];
        height = origin.height;
        width = origin.width;
    }
    channel = origin.channel;
    concentration = origin.concentration;
    for (std::size_t i = 0; i < height * width; ++i) {
        framebuffer[i] = origin.framebuffer[i];
    }
    return *this;
}

RGBImage::~RGBImage() {
    delete[] framebuffer;
    delete[] energyMap;
    framebuffer = nullptr;
    energyMap = nullptr;
}

void RGBImage::write(const std::string &path) {
    auto *raw_data = new uchar[width * height * channel];
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

ST_TWO_FLOAT &RGBImage::atEnergyMap(std::size_t i, std::size_t j) const {
    return energyMap[i * width + j];
}

RGBPixel &RGBImage::at(std::size_t i, std::size_t j) const {
    return framebuffer[i * width + j];
}

float RGBImage::evaluateEnergyAt(std::size_t i, std::size_t j) const {
    // Energy form "e1" is implemented here.
    // Heuristically, we use grey channel to evaluate the energy
    auto curr = at(i, j).grey();
    auto deltaI = (i == height - 1 ? at(0, j) : at(i + 1, j)).grey();
    auto deltaJ = (j == width - 1 ? at(i, 0) : at(i, j + 1)).grey();
    auto gamma = std::accumulate(concentration.begin(), concentration.end(), 1.f,
                                 [&](const auto &c1, const auto &c2) -> float {
                                     return c1 * evaluateGamma(i, j, c2);
                                 });
    return gamma * (std::abs(deltaI - curr) + std::abs(deltaJ - curr));
}

std::vector<Seam_Path> RGBImage::combVertical(std::size_t capacity) {
    std::vector<Seam_Path> ret{};
    for (std::size_t k = 0; k < capacity; ++k) {
        auto path = combVertical_exclusive(ret);
        ret.push_back(path);
    }
    return ret;
}

Seam_Path RGBImage::combVertical_exclusive(const std::vector<Seam_Path> &ex) {
    // Check if position (i,j) collide with the paths in ex
    auto checker = [&](std::size_t vi, std::size_t vj) -> bool {
        // Pixel in "path.path" are of the order of descending of i-index.
        return std::any_of(ex.begin(), ex.end(), [&](const auto &ele) {
            return ele.path[height - vi - 1].j == vj;
        });
    };
    // DP: Initialize the starting state
    for (std::size_t j = 0; j < width; ++j) {
        atEnergyMap(0, j).ver = evaluateEnergyAt(0, j);
    }
    // DP: Iterate through the energy map
    for (std::size_t i = 1; i < height; ++i) {
        for (std::size_t j = 0; j < width; ++j) {
            if (checker(i, j)) { // Check colliding
                continue;
            }
            float tmp_min = FLT_MAX;
            std::size_t tmp_prev = -1;
            for (int d = -1; d <= 1; ++d) {
                if (static_cast<int>(j) + d >= 0 && static_cast<int>(j) + d < width
                    && !checker(i - 1, j + d)
                    && tmp_min > evaluateEnergyAt(i, j) + atEnergyMap(i - 1, j + d).ver) {
                    tmp_min = evaluateEnergyAt(i, j) + atEnergyMap(i - 1, j + d).ver;
                    tmp_prev = j + d;
                }
            }
            atEnergyMap(i, j).ver = tmp_min;
            atEnergyMap(i, j).prev_j = tmp_prev;
        }
    }
    float goal_path_energy = FLT_MAX;
    std::size_t goal_path_j = -1;
    Seam_Path ret{};
    for (std::size_t j = 0; j < width; ++j) {
        if (!checker(height - 1, j) && atEnergyMap(height - 1, j).ver < goal_path_energy) {
            goal_path_energy = atEnergyMap(height - 1, j).ver;
            goal_path_j = j;
        }
    }
    ret.energy = goal_path_energy;
    for (std::size_t i = 0; i < height; ++i) {
        ret.path.push_back({height - i - 1, goal_path_j});
        goal_path_j = atEnergyMap(height - i - 1, goal_path_j).prev_j;
    }
    return ret;
}

std::vector<Seam_Path> RGBImage::combHorizontal(std::size_t capacity) {
    std::vector<Seam_Path> ret{};
    for (std::size_t k = 0; k < capacity; ++k) {
        auto path = combHorizontal_exclusive(ret);
        ret.push_back(path);
    }
    return ret;
}

Seam_Path RGBImage::combHorizontal_exclusive(const std::vector<Seam_Path> &ex) {
    auto checker = [&](std::size_t vi, std::size_t vj) -> bool {
        return std::any_of(ex.begin(), ex.end(), [&](const auto &ele) {
            return ele.path[width - vj - 1].i == vi;
        });
        return false;
    };
    for (std::size_t i = 0; i < height; ++i) {
        auto &ttt = atEnergyMap(i, 0);
        atEnergyMap(i, 0).hor = evaluateEnergyAt(i, 0);
    }
    for (std::size_t j = 1; j < width; ++j) {
        for (std::size_t i = 0; i < height; ++i) {
            if (checker(i, j)) {
                continue;
            }
            float tmp_min = FLT_MAX;
            std::size_t tmp_prev = -1;
            for (int d = -1; d <= 1; ++d) {
                if (static_cast<int>(i) + d >= 0 && static_cast<int>(i) + d < height
                    && !checker(i + d, j - 1)
                    && tmp_min > evaluateEnergyAt(i, j) + atEnergyMap(i + d, j - 1).hor) {
                    tmp_min = evaluateEnergyAt(i, j) + atEnergyMap(i + d, j - 1).hor;
                    tmp_prev = i + d;
                }
            }
            atEnergyMap(i, j).hor = tmp_min;
            atEnergyMap(i, j).prev_i = tmp_prev;
        }
    }
    float goal_path_energy = FLT_MAX;
    std::size_t goal_path_i = -1;
    Seam_Path ret{};
    for (std::size_t i = 0; i < height; ++i) {
        if (!checker(i, width - 1) && atEnergyMap(i, width - 1).hor < goal_path_energy) {
            goal_path_energy = atEnergyMap(i, width - 1).hor;
            goal_path_i = i;
        }
    }
    ret.energy = goal_path_energy;
    for (std::size_t j = 0; j < width; ++j) {
        ret.path.push_back({goal_path_i, width - j - 1});
        goal_path_i = atEnergyMap(goal_path_i, width - j - 1).prev_i;
    }
    return ret;
}

std::size_t RGBImage::getOffset(const PixelPos &pos) const {
    return pos.i * width + pos.j;
}

std::size_t RGBImage::getOffset(std::size_t i, std::size_t j) const {
    return i * width + j;
}

void RGBImage::rescale(std::size_t newHeight, std::size_t newWidth) {
    auto heightShrinkFlag = newHeight < height, widthShrinkFlag = newWidth < width;
    if (heightShrinkFlag && widthShrinkFlag) {
        std::cout << "Scaling down will take much more time, here is its progress." << std::endl;
        rescale_scaleDown(newHeight, newWidth);
    } else if (heightShrinkFlag) {
        // Case 1.1
        for (std::size_t k = 0; k < height - newHeight; ++k) {
            collapseHorizontalSeam(combHorizontal(1).at(0));
        }
        auto pathVer = combVertical(newWidth - width);
        repeatVerticalSeam(pathVer);
    } else if (widthShrinkFlag) {
        // Case 1.2
        for (std::size_t k = 0; k < width - newWidth; ++k) {
            collapseVerticalSeam(combVertical(1).at(0));
        }
        auto pathHor = combHorizontal(newHeight - height);
        repeatHorizontalSeam(pathHor);
    } else {
        // Case 2
        auto pathVer = combVertical(newWidth - width);
        repeatVerticalSeam(pathVer);
        auto pathHor = combHorizontal(newHeight - height);
        repeatHorizontalSeam(pathHor);
    }
}

void RGBImage::rescale_scaleDown(std::size_t h, std::size_t w) {
    auto heightShrink = height - h, widthShrink = width - w;
    std::vector<std::pair<float, RGBImage>> curr_state, next_state;
    // DP: buildup the initial state
    curr_state.emplace_back(0.f, *this);
    for (std::size_t k = 1; k < widthShrink; ++k) {
        std::cout << "Processing: " << k << '/' << heightShrink * widthShrink - 1;
        curr_state.push_back(curr_state.at(k - 1));
        auto path = curr_state.at(k).second.combVertical(1).at(0);
        curr_state.at(k).first = path.energy;
        curr_state.at(k).second
                .collapseVerticalSeam(path);
        std::cout << ", " << path.energy << std::endl;
    }
    // DP: Transmission
    for (std::size_t t = 1; t < heightShrink; ++t) {
        next_state.clear();
        next_state.push_back(curr_state.at(0));
        auto tmp_path = next_state.at(0).second.combHorizontal(1).at(0);
        next_state.at(0).first = tmp_path.energy + curr_state.at(0).first;
        next_state.at(0).second.collapseHorizontalSeam(tmp_path);
        for (std::size_t k = 1; k < widthShrink; ++k) {
            std::cout << "Processing: " << t * widthShrink + k << '/' << heightShrink * widthShrink - 1;
            auto pathHor = curr_state.at(k).second.combHorizontal(1).at(0);
            auto pathVer = next_state.at(k - 1).second.combVertical(1).at(0);
            auto min_energy = std::min(pathHor.energy, pathVer.energy);
            if (pathHor.energy + curr_state.at(k).first < pathVer.energy + next_state.at(k - 1).first) {
                next_state.emplace_back(pathHor.energy + curr_state.at(k).first, curr_state.at(k).second);
                next_state.at(k).second.collapseHorizontalSeam(pathHor);
            } else {
                next_state.emplace_back(pathVer.energy + next_state.at(k - 1).first, next_state.at(k - 1).second);
                next_state.at(k).second.collapseVerticalSeam(pathVer);
            }
            std::cout << ", " << min_energy << std::endl;
        }
        curr_state.clear();
        std::swap(curr_state, next_state);
    }
    *this = curr_state.at(widthShrink - 1).second;
}

// The following two functions both run with the complexity of O(mn), where "m" and "n" is the height and width of the
// image.

void RGBImage::collapseVerticalSeam(const Seam_Path &seamPath) {
    // "seamPath" should be a vertical seam from top to bottom.
    /* Since "seamPath" vector is lined in the descent order of member ".i", so we can iterate through "seamPath" from
     * tail to head for efficient move.
     * Type "int" used here. */
    for (auto i = static_cast<int>(height) - 1; i >= 0; --i) {
        auto range_start = getOffset(seamPath.path[i]) + 1;
        auto range_end = i ? getOffset(seamPath.path[i - 1]) : height * width;
        for (auto k = range_start; k < range_end; ++k) {
            framebuffer[k - (height - i)] = framebuffer[k];
        }
    }
    // Update the concentrations
    for (auto &cons: concentration) {
        if (seamPath.path[height - cons.pixelPos.i - 1].j <= cons.pixelPos.j) {
            --cons.pixelPos.j;
        }
    }
    --width;
}

void RGBImage::collapseHorizontalSeam(const Seam_Path &seamPath) {
    // "seamPath" should be a vertical seam from left to right.
    /* Because the image is stored left to right then top to down, so the strategy applied to collapse the vertical seam
     * doesn't work in this case. We can only iterate through each pixel in the image with the same order and process
     * every one of then correspondingly.
     * If we want to use that strategy, the code would be quite complex. For convenience consideration, plain iteration
     * is implemented. */
    for (std::size_t i = 0; i < height; ++i) {
        for (std::size_t j = 0; j < width; ++j) {
            if (i > seamPath.path[width - j - 1].i) {
                at(i - 1, j) = at(i, j);
            }
        }
    }
    // Update the concentrations
    for (auto &cons: concentration) {
        if (seamPath.path[width - cons.pixelPos.j - 1].i <= cons.pixelPos.i) {
            --cons.pixelPos.i;
        }
    }
    --height;
}

void RGBImage::repeatVerticalSeam(const std::vector<Seam_Path> &seamPath) {
    // Check if position (i,j) collide with the paths in seamPath
    auto checker = [&](std::size_t vi, std::size_t vj) -> bool {
        return std::any_of(seamPath.begin(), seamPath.end(), [&](const auto &ele) {
            return ele.path[height - vi - 1].j == vj;
        });
    };
    // Reallocate the space for enlarge
    auto newHeight = height, newWidth = seamPath.size() + width;
    auto newBuffer = new RGBPixel[newHeight * newWidth];
    // The offset each pixel of each row should move. At the end of each row, this value must be seamPath.size().
    std::size_t pixelOffset = -1;
    for (std::size_t vi = 0; vi < height; ++vi) {
        pixelOffset = seamPath.size();
        for (std::size_t vj = 0; vj < width; ++vj) {
            auto i = height - vi - 1, j = width - vj - 1;
            // Do the offset
            newBuffer[getOffset(i, j) + i * seamPath.size() + pixelOffset] = framebuffer[getOffset(i, j)];
            if (checker(i, j)) {
                --pixelOffset;
                newBuffer[getOffset(i, j) + i * seamPath.size() + pixelOffset] = framebuffer[getOffset(i, j)];
            }
        }
    }
    delete[] framebuffer;
    delete[] energyMap;
    framebuffer = newBuffer;
    energyMap = new ST_TWO_FLOAT[newHeight * newWidth];
    // Update concentrations
    for (auto &path: seamPath) {
        for (auto &cons: concentration) {
            if (path.path[height - cons.pixelPos.i - 1].j <= cons.pixelPos.j) {
                ++cons.pixelPos.j;
            }
        }
    }

    height = newHeight;
    width = newWidth;
}

void RGBImage::repeatHorizontalSeam(std::vector<Seam_Path> &seamPath) {
    transpose();
    for (auto &path: seamPath) {
        for (auto &pixel: path.path) {
            std::swap(pixel.i, pixel.j);
        }
    }
    for (auto &cons: concentration) {
        std::swap(cons.pixelPos.i, cons.pixelPos.j);
    }
    repeatVerticalSeam(seamPath);
    transpose();
    for (auto &path: seamPath) {
        for (auto &pixel: path.path) {
            std::swap(pixel.i, pixel.j);
        }
    }
    for (auto &cons: concentration) {
        std::swap(cons.pixelPos.i, cons.pixelPos.j);
    }
}

void RGBImage::transpose() {
    auto newBuffer = new RGBPixel[height * width];
    for (std::size_t i = 0; i < height; ++i) {
        for (std::size_t j = 0; j < width; ++j) {
            newBuffer[j * height + i] = framebuffer[i * width + j];
        }
    }
    delete[] framebuffer;
    framebuffer = newBuffer;

    std::swap(height, width);
}

float RGBImage::evaluateDistanceBetween(const PixelPos &a, const PixelPos &b) {
    return std::max(std::abs(static_cast<float>(a.i - b.i)), std::abs(static_cast<float>(a.j - b.j)));
}

float RGBImage::evaluateGamma(const PixelPos &pixelPos, const Concentration &concentration) {
    return evaluateGamma(pixelPos.i, pixelPos.j, concentration);
}

float RGBImage::evaluateGamma(std::size_t i, std::size_t j, const Concentration &concentration) {
    auto distance = std::max(std::abs(static_cast<float>((int) i - (int) concentration.pixelPos.i)),
                             std::abs(static_cast<float>((int) j - (int) concentration.pixelPos.j)));
//    auto gamma = std::max(1.f,
//                          concentration.enhancement *
//                          (std::exp(-distance) * std::exp(concentration.range) - 1) + 1);
    auto gamma = distance < concentration.range ? concentration.enhancement : 1.f;
    return gamma;
}

void RGBImage::writeEnergyImage(const std::string &path) const {
    RGBImage ri(*this);
    for (std::size_t i = 0; i < height; ++i) {
        for (std::size_t j = 0; j < width; ++j) {
            auto t = static_cast<uchar>(std::atan(evaluateEnergyAt(i, j) * 2.f) * 2.f / 3.1415f * 255.f);
            ri.at(i, j) = {t, t, t, 100};
        }
    }
    ri.write(path);
}