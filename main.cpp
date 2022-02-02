//
// Created by mine268 on 2022/1/29.
//

#define SEAM_CARVING_DEBUG
#include "RGBImage.h"
#include <iostream>

int main() {
    RGBImage ri("../pictures/sample5.png");

    ri.rescale(100, 200);

    ri.write("../outputs/output5_1.png");

    return 0;
}
