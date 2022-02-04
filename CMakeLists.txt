cmake_minimum_required(VERSION 3.12)
project(Image_rescaling LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp RGBImage.cpp)