#include "png.hpp"
#include "ico.hpp"

#include <iostream>

static inline void print_help()
{
    std::cout << "Usage: png2ico iconfile.ico pngfile1 [pngfile2 ...]\n";
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        std::cout << "No input files specified\n";
        print_help();
        return -1;
    }

    std::vector<png_image> png;
    for (int index = 2; index < argc; index++)
    {
        png_image image;
        if (!read_png_file(argv[index], &image))
            return -1;
        png.push_back(image);
    }

    if (!write_ico_file(argv[1], png))
        return -1;

    return 0;
}
