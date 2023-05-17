#include "ico.hpp"
#include "png.hpp"

#include <iostream>
#include <fstream>

bool write_ico_file(const char* path, const std::vector<png_image>& png)
{
    std::cout << "Writing ico file to " << path << "...\n";

    std::ofstream ico_stream(path, std::ios::binary);
    if (!ico_stream.is_open())
    {
        std::cerr << "Error, unable to open " << path << "\n";
        return false;
    }

    constexpr auto ICO_RESOURCE_TYPE = 1;
    const icon_header header = {
        0,
        ICO_RESOURCE_TYPE,
        static_cast<uint16_t>(png.size())
    };

    if (!ico_stream.write(reinterpret_cast<const char*>(&header), sizeof(header)))
    {
        std::cerr << "Error, unable to write ico header to " << path << "\n";
        return false;
    }

    uint32_t offset = (sizeof(icon_header) + (sizeof(icon_entry) * header.count));
    for (auto& image : png)
    {
        std::cout << " Adding png width " << image.width
                  << ", height " << image.height
                  << ", bit depth " << static_cast<uint16_t>(image.bit_depth)
                  << "...\n";

        const icon_entry entry = {
            (image.width > 255) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(image.width),
            (image.height > 255) ? static_cast<uint8_t>(0) : static_cast<uint8_t>(image.height),
            0, // color palette
            0, // reserved
            0, // color plane
            static_cast<uint16_t>(image.bit_depth),
            image.size,
            offset
        };

        if (!ico_stream.write(reinterpret_cast<const char*>(&entry), sizeof(entry)))
        {
            std::cerr << "Error while writing ico entry to " << path << "\n";
            return false;
        }

        offset += entry.size;
    }

    for (auto& image : png)
    {
        if (!ico_stream.write(reinterpret_cast<const char*>(image.data.data()), image.size))
        {
            std::cerr << "Error while writing PNG raw data to " << path << "\n";
            return false;
        }
    }

    std::cout << "Done.\n";

    return true;
}
