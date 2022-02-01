//
// Created by mine268 on 2022/1/29.
//

#pragma once

#include "RGBPixel.h"
#include <cfloat>
#include <string>
#include <vector>

using uchar = unsigned char;

/**
 * @brief Structure that holds two float, for implementation of dynamic programming.
 */
struct ST_TWO_FLOAT {
    float ver{FLT_MAX}, hor{FLT_MAX};
    std::size_t prev_i{static_cast<size_t>(-1)}, prev_j{static_cast<size_t>(-1)};
};

/**
 * @brief The path that consisted of connected pixel.
 */
struct Seam_Path {
    std::vector<PixelPos> path{}; // The pixels in the path
    float energy{FLT_MAX}; // The energy of the path
};

/**
 * RGBImage class for image reading and writing.
 * _WARN_: No out-of-bound error is checked in the code.
 */
class RGBImage {
#ifdef SEAM_CARVING_DEBUG
    friend int main();
#endif
private:
    ST_TWO_FLOAT *energyMap; // Energy map for convenience of evaluating minimum energy path.

    /**
     * Return the cumulated energy stored at point(i,j).
     * @param i Row(Vertical) index.
     * @param j Column(Horizontal) index.
     * @return Cumulated energy stored at point(i,j).
     */
    ST_TWO_FLOAT & atEnergyMap(std::size_t i, std::size_t j) const;

    /**
     * @brief Get the offset of pixel in the "framebuffer".
     * @param pos Position in form of (i,j).
     * @return The offset in the "framebuffer".
     */
    std::size_t getOffset(const PixelPos& pos) const;

    /**
     * @brief Get the offset of pixel in the "framebuffer".
     * @param i Row index.
     * @param j Column index.
     * @return The offset in the "framebuffer".
     */
    std::size_t getOffset(std::size_t i, std::size_t j) const;

    /**
     * @brief Delete the given seam-path, shifting all the pixels right of it left.
     * @param seamPath Seam path.
     */
    void collapseVerticalSeam(const Seam_Path & seamPath);

    /**
     * @brief Delete the given seam-path, shifting all the pixels down of it above.
     * @param seamPath Seam path.
     */
    void collapseHorizontalSeam(const Seam_Path & seamPath);

    /**
     * @brief Repeat the given seam-path, shifting all the pixels right of it right.
     * @param seamPath Seam path.
     */
    void repeatVerticalSeam(const std::vector<Seam_Path> & seamPath);

    /**
     * @brief Repeat the given seam-path, shifting all the pixels down of it down. Implementing by transpose, so
     * "seamPath" is non-constant. Space cost is high for convenience of lower coding complexity. (It fucks!)
     * @param seamPath Seam path.
     */
    void repeatHorizontalSeam(std::vector<Seam_Path> & seamPath);

    /**
     * @brief Comb through the image to find a minimum energy path that has no same pixel as the paths in "ex", called
     * exclusive path set.
     * @param ex Exclusive path set.
     * @return The path.
     */
    Seam_Path combVertical_exclusive(const std::vector<Seam_Path> &ex);

    /**
     * @brief Comb through the image to find a minimum energy path that has no same pixel as the paths in "ex", called
     * exclusive path set.
     * @param ex Exclusive path set.
     * @return The path.
     */
    Seam_Path combHorizontal_exclusive(const std::vector<Seam_Path> &ex);

    /**
     * @brief Rescale to (h,w) where h is smaller than "height" and w is smaller than "width".
     * @param h New height.
     * @param w New width.
     */
    void rescale_scaleDown(std::size_t h, std::size_t w);

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
    virtual RGBPixel & at(std::size_t i, std::size_t j) const;

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
      * @param capacity Most "capacity" minimum path.
      * @return Found paths.
      */
     std::vector<Seam_Path> combVertical(std::size_t capacity);

     /**
      * @brief To find a horizontal path with minimum energy cost.
      * @param capacity Most "capacity" minimum path.
      * @return Found paths.
      */
     std::vector<Seam_Path> combHorizontal(std::size_t capacity);

     /**
      * @brief Transpose the image. This function is of high space complexity and should be used carefully!
      */
     void transpose();

     /**
      * @brief Implementing seam-carving algorithm to rescale the image to given aspect ratio.
      * @param newHeight New height.
      * @param newWidth New width.
      */
     void rescale(std::size_t newHeight, std::size_t newWidth);
};
