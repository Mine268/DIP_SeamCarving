//
// Created by mine268 on 2022/1/29.
//

#define SEAM_CARVING_DEBUG

#include "RGBImage.h"
#include <iostream>

int main() {
    RGBImage ri("../pictures/sample5.png");

    // width: 311, height: 207
    ri.rescale(200, 300);

    ri.write("../outputs/output5.png");

    return 0;
}
