//
// Created by mine268 on 2022/1/29.
//

#include "RGBImage.h"
#include <iostream>

int main(void) {
    RGBImage ri("../pictures/sample1.png");
    ri.write("../outputs/output1.png");

    return 0;
}