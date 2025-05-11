#include <cstdint>
#include <vector>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>

constexpr auto PNG_SIGNATURE_SIZE = 8;
constexpr auto PNG_CHUNK_TYPE_SIZE = 4;
constexpr auto PNG_IHDR_CHUNK_SIZE = 13;

struct PngImage
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t bit_depth = 0;
    uint32_t size = 0;
    std::vector<uint8_t> data;
};

struct IcoHeader
{
    uint16_t reserved = 0;
    uint16_t type = 0;
    uint16_t count = 0;
};

struct IcoEntry
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

inline uint32_t swap_uint32(uint32_t value)
{
    value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    return (value << 16) | (value >> 16);
}

inline bool check_png_signature(const uint8_t* signature)
{
    const uint8_t png_signature[PNG_SIGNATURE_SIZE] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
    return (std::memcmp(signature, png_signature, sizeof(png_signature)) == 0);
}

inline bool check_ihdr_chunk(const uint8_t* chunk)
{
    const uint8_t ihdr_chunk[PNG_CHUNK_TYPE_SIZE] = { 0x49, 0x48, 0x44, 0x52 };
    return (std::memcmp(chunk, ihdr_chunk, sizeof(ihdr_chunk)) == 0);
}

inline uint32_t get_file_size(const char* file_path)
{
    std::filesystem::path path { file_path };
    return std::filesystem::file_size(path);
}

bool read_png_file(const char* file_path, PngImage* png)
{
    assert(png);

    std::cout << "Info: Reading PNG image from " << file_path << "...\n";

    std::ifstream stream(file_path, std::ios::binary);
    if (!stream.is_open())
    {
        std::cerr << "Error: Cannot open file.\n";
        return false;
    }

    png->size = get_file_size(file_path);

    uint8_t signature[PNG_SIGNATURE_SIZE] = { 0 };
    if (!stream.read(reinterpret_cast<char*>(signature), sizeof(signature)))
    {
        std::cerr << "Error: Cannot read PNG signature, not a PNG image.\n";
        return false;
    }

    if (!check_png_signature(signature))
    {
        std::cerr << "Error: Invalid PNG signature, not a PNG image.\n";
        return false;
    }

    uint32_t chunk_size = 0;
    if (!stream.read(reinterpret_cast<char*>(&chunk_size), sizeof(chunk_size)))
    {
        std::cerr << "Error: Cannot read PNG chunk size.\n";
        return false;
    }

    chunk_size = swap_uint32(chunk_size);

    uint8_t chunk[PNG_CHUNK_TYPE_SIZE] = { 0 };
    if (!stream.read(reinterpret_cast<char*>(chunk), sizeof(chunk)))
    {
        std::cerr << "Error: Cannot read PNG chunk type.\n";
        return false;
    }

    if (!check_ihdr_chunk(chunk) && chunk_size != PNG_IHDR_CHUNK_SIZE)
    {
        std::cerr << "Error: Unsupported PNG image format.\n";
        return false;
    }

    uint32_t width = 0;
    uint32_t height = 0;

    if (!stream.read(reinterpret_cast<char*>(&width), sizeof(width)))
    {
        std::cerr << "Error: Cannot read PNG image width.\n";
        return false;
    }

    if (!stream.read(reinterpret_cast<char*>(&height), sizeof(height)))
    {
        std::cerr << "Error: Cannot read PNG image height.\n";
        return false;
    }

    png->width = swap_uint32(width);
    png->height = swap_uint32(height);

    if (png->width > 512 || png->width != png->height)
    {
        std::cerr << "Error: Invalid PNG image size width "
                  << png->width << ", height " << png->height
                  << ".\n  PNG width and height should be equal and smaller than 512 pixels.\n";
        return false;
    }

    if (!stream.read(reinterpret_cast<char*>(&png->bit_depth), sizeof(png->bit_depth)))
    {
        std::cerr << "Error: Cannot read PNG color depth.\n";
        return false;
    }

    stream.seekg(std::ios::beg);
    png->data.reserve(png->size);
    png->data.insert(png->data.begin(),
                     std::istreambuf_iterator<char>(stream),
                     std::istreambuf_iterator<char>());

    return true;
}

bool write_ico_file(const char* file_path, const std::vector<PngImage>& png)
{
    std::cout << "Info: Writing ICO file to " << file_path << "...\n";

    std::ofstream stream(file_path, std::ios::binary);
    if (!stream.is_open())
    {
        std::cerr << "Error: Unable to create file.\n";
        return false;
    }

    constexpr auto ICO_RESOURCE_TYPE = 1;
    const IcoHeader header = {
        0,
        ICO_RESOURCE_TYPE,
        static_cast<uint16_t>(png.size())
    };

    if (!stream.write(reinterpret_cast<const char*>(&header), sizeof(header)))
    {
        std::cerr << "Error: Unable to write ICO header.\n";
        return false;
    }

    uint32_t offset = (sizeof(IcoHeader) + (sizeof(IcoEntry) * header.count));

    for (auto& image : png)
    {
        std::cout << "Info: Adding PNG image, width: " << image.width
                  << ", height: " << image.height
                  << ", bit depth: " << static_cast<uint16_t>(image.bit_depth)
                  << "...\n";

        const IcoEntry entry = {
            (image.width > 255) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(image.width),
            (image.height > 255) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(image.height),
            0, // color palette
            0, // reserved
            0, // color plane
            static_cast<uint16_t>(image.bit_depth),
            image.size,
            offset
        };

        if (!stream.write(reinterpret_cast<const char*>(&entry), sizeof(entry)))
        {
            std::cerr << "Error: Cannot write ICO entry.\n";
            return false;
        }

        offset += entry.size;
    }

    for (auto& image : png)
    {
        if (!stream.write(reinterpret_cast<const char*>(image.data.data()), image.size))
        {
            std::cerr << "Error: Cannot write PNG raw data.\n";
            return false;
        }
    }

    std::cout << "Done.\n";

    return true;
}

inline void print_help()
{
    std::cout << "Usage: png2ico iconfile.ico pngfile1.png [pngfile2.png ...]\n";
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "No input files specified\n";
        print_help();
        return -1;
    }

    std::vector<PngImage> png;
    for (int index = 2; index < argc; index++)
    {
        PngImage image;
        if (!read_png_file(argv[index], &image))
            return -1;
        png.push_back(image);
    }

    if (!write_ico_file(argv[1], png))
        return -1;

    return 0;
}
