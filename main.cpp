//
// Created by mine268 on 2022/1/29.
//

#define SEAM_CARVING_DEBUG
#include "RGBImage.h"
#include <iostream>

int main() {
    RGBImage ri("../pictures/sample1.png");

    for (int i = 0; i < 100; i++) {
        auto path = ri.combHorizontal();
        ri.collapseHorizontalSeam(path);
    }

    ri.write("../outputs/output1.png");

    return 0;
}
