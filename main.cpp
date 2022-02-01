//
// Created by mine268 on 2022/1/29.
//

#define SEAM_CARVING_DEBUG
#include "RGBImage.h"
#include <iostream>

int main() {
    RGBImage ri("../pictures/sample5.png");

    auto paths = ri.combHorizontal(3);
    for (const auto& path : paths) {
        for (auto pixel : path.path) {
            ri.at(pixel.i, pixel.j) = {255, 0, 0, 255};
        }
    }

    ri.write("../outputs/output5.png");

    return 0;
}
