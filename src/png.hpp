#pragma once

#include <cstdint>
#include <vector>

struct png_image
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t bit_depth = 0;
    uint32_t size = 0;
    std::vector<uint8_t> data;
};

bool read_png_file(const char* path, png_image* png);
