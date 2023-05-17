#include "png.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

constexpr auto PNG_SIGNATURE_SIZE = 8;
constexpr auto PNG_CHUNK_TYPE_SIZE = 4;
constexpr auto PNG_IHDR_CHUNK_SIZE = 0xD;

static inline uint32_t swap_uint32(uint32_t value)
{
    value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    return (value << 16) | (value >> 16);
}

static inline bool check_png_signature(const uint8_t* sig)
{
    const uint8_t png_signature[PNG_SIGNATURE_SIZE] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    return (std::memcmp(sig, png_signature, sizeof(png_signature)) == 0);
}

static inline bool check_ihdr_chunk(const uint8_t* chunk)
{
    const uint8_t ihdr_chunk[PNG_CHUNK_TYPE_SIZE] = { 0x49, 0x48, 0x44, 0x52 };
    return (std::memcmp(chunk, ihdr_chunk, sizeof(ihdr_chunk)) == 0);
}

bool read_png_file(const char* path, png_image* png)
{
    if (!png)
        return false;

    std::ifstream png_stream(path, std::ios::binary);
    if (!png_stream.is_open())
    {
        std::cerr << "Error, cannot open " << path << "\n";
        return false;
    }

    png->size = get_file_size(path);

    uint8_t signature[PNG_SIGNATURE_SIZE] = { 0 };
    if (!png_stream.read(reinterpret_cast<char*>(signature), sizeof(signature)))
    {
        std::cerr << "Error while reading file signature from " << path << "\n";
        return false;
    }

    if (!check_png_signature(signature))
    {
        std::cerr << "Error, " << path << " is not a PNG image\n";
        return false;
    }

    uint32_t chunk_size = 0;
    if (!png_stream.read(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size)))
    {
        std::cerr << "Error while reading PNG chunk size from " << path << "\n";
        return false;
    }

    chunk_size = swap_uint32(chunk_size);

    uint8_t chunk[PNG_CHUNK_TYPE_SIZE] = { 0 };
    if (!png_stream.read(reinterpret_cast<char*>(chunk), sizeof(chunk)))
    {
        std::cerr << "Error while reading png chunk type from " << path << "\n";
        return false;
    }

    if (!check_ihdr_chunk(chunk) && chunk_size != PNG_IHDR_CHUNK_SIZE)
    {
        std::cerr << "Error, " << path << " unsupported PNG format\n";
        return false;
    }

    uint32_t width = 0;
    if (!png_stream.read(reinterpret_cast<char*>(&width), sizeof(width)))
    {
        std::cerr << "Error while reading png width from " << path << "\n";
        return false;
    }
    png->width = swap_uint32(width);

    uint32_t height = 0;
    if (!png_stream.read(reinterpret_cast<char*>(&height), sizeof(height)))
    {
        std::cerr << "Error while reading png height from " << path << "\n";
        return false;
    }
    png->height = swap_uint32(height);

    if (png->width > 512 || png->width != png->height)
    {
        std::cerr << "Error, invalid png image size width "
                  << png->width << ", height " << png->height
                  << ".\n  PNG width and height should be equal and smaller than 512 pixels.\n";
        return false;
    }

    if (!png_stream.read(reinterpret_cast<char*>(&png->bit_depth), sizeof(png->bit_depth)))
    {
        std::cerr << "Error while reading png color depth from " << path << "\n";
        return false;
    }

    png_stream.seekg(std::ios::beg);
    png->data.reserve(png->size);
    png->data.insert(png->data.begin(),
                     std::istreambuf_iterator<char>(png_stream),
                     std::istreambuf_iterator<char>());

    return true;
}
