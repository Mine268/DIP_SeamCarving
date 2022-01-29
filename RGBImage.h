//
// Created by mine268 on 2022/1/29.
//

#pragma once

#include "RGBPixel.h"
#include <string>
#include <vector>

using uchar = unsigned char;

/**
 * @brief Structure that holds two float, for implementation of dynamic programming.
 */
struct ST_TWO_FLOAT {
    float ver, hor;
    std::size_t prev_i, prev_j;
};

/**
 * @brief The path that consisted of connected pixel.
 */
struct Seam_Path {
    std::vector<PixelPos> path; // The pixels in the path
    float energy; // The energy of the path
};

/**
 * RGBImage class for image reading and writing.
 * _WARN_: No out-of-bound error is checked in the code.
 */
class RGBImage {
private:
    ST_TWO_FLOAT *energyMap; // Energy map for convenience of evaluating minimum energy path.

    /**
     * Return the cumulated energy stored at point(i,j).
     * @param i Row(Vertical) index.
     * @param j Column(Horizontal) index.
     * @return Cumulated energy stored at point(i,j).
     */
    ST_TWO_FLOAT & atEnergyMap(std::size_t i, std::size_t j);

public:
    std::size_t height{0}, width{0}, channel{0}; // height & width of the image.
    RGBPixel *framebuffer; // Buffer that stored the image in RGB format

    RGBImage() = delete;
    explicit RGBImage(std::size_t h, std::size_t w);
    explicit RGBImage(const std::string & path);
    ~RGBImage();

    /**
     * @brief Return the pixel at point (i,j).
     * @param i Row(Vertical) index.
     * @param j Column(Horizontal) index.
     * @return The pixel at point (i,j).
     */
    virtual RGBPixel & at(std::size_t i, std::size_t j);

    /**
     * @brief Write the image to the path. PNG format.
     * @param path Path of the image.
     */
     virtual void write(const std::string & path);

     /**
      * @brief Return the energy value at point (i,j).
      * @param i Row(Vertical) index.
      * @param j Column(Horizontal) index.
      * @return Energy value at point (i,j).
      */
     virtual float evaluateEnergyAt(std::size_t i, std::size_t j);

     /**
      * @brief To find a vertical path with minimum energy cost.
      * @return Found path.
      */
     Seam_Path combVertical();

     /**
      * @brief To find a horizontal path with minimum energy cost.
      * @return Found path.
      */
     Seam_Path combHorizontal();
};
