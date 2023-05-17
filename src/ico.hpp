#pragma once

#include <cstdint>
#include <vector>

struct icon_header
{
    uint16_t reserved = 0;
    uint16_t type = 0;
    uint16_t count = 0;
};

struct icon_entry
{
    uint8_t width = 0;
    uint8_t height = 0;
    uint8_t color_count = 0;
    uint8_t reserved = 0;
    uint16_t planes = 0;
    uint16_t bit_count = 0;
    uint32_t size = 0;
    uint32_t offset = 0;
};

struct png_image;

bool write_ico_file(const char* path, const std::vector<png_image>& png);
