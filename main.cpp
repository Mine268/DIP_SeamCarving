//
// Created by mine268 on 2022/1/29.
//

#define SEAM_CARVING_DEBUG

#include "RGBImage.h"
#include <iostream>

int main(int argc, const char **argv) {

    if (argc < 5) {
        std::cout
                << "Usage: source_path height width output_path [{range enhancement concentration_i concentration_j}] ..."
                << std::endl
                << "Output format: png" << std::endl;
    } else if ((argc - 5) % 4 != 0) {
        std::cout << "Please check your concentrations!" << std::endl;
    } else {
        auto src = std::string(argv[1]);
        auto dest = std::string(argv[4]);
        auto height = std::strtoull(argv[2], nullptr, 10) - 1;
        auto width = std::strtoull(argv[3], nullptr, 10) - 1;
        RGBImage ri(src);

        for (std::size_t i = 0; i < argc - 5; i += 4) {
            ri.concentration.push_back({static_cast<float>(std::strtof(argv[5 + i], nullptr)),
                                        static_cast<float>(std::strtof(argv[6 + i], nullptr)),
                                        {std::strtoull(argv[7 + i], nullptr, 10),
                                         std::strtoull(argv[8 + i], nullptr, 10)}});
        }

        ri.rescale(height, width);
        ri.write(dest);
    }

    return 0;
}
