//
// Created by mine268 on 2022/1/29.
//

#include "RGBImage.h"
#include <iostream>

int main(int argc, const char **argv) {

    if (argc != 5) {
        std::cout << "Usage: source_path height width output_path" << std::endl << "Output format: png";
    } else {
        auto src = std::string(argv[1]);
        auto dest = std::string(argv[4]);
        auto height = std::strtoull(argv[2], nullptr, 10);
        auto width = std::strtoull(argv[3], nullptr, 10);

        RGBImage ri(src);
        ri.rescale(height, width);
        ri.write(dest);
    }

    return 0;
}
