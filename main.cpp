//
// Created by mine268 on 2022/1/29.
//

#include "RGBImage.h"
#include <iostream>

int main(void) {
    RGBImage ri("../pictures/sample1.png");
    auto path = ri.combHorizontal();
    for (auto px : path.path) {
        ri.at(px.i, px.j) = {0, 0, 0, 255};
    }
    ri.write("../outputs/output1.png");

    return 0;
}
