# png2ico

Create Windows icon file from a list of PNG images.

Images are be stored in the PNG format inside the ico file with a small entry header (16 bytes):

```C
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
```

## Windows with MSVC

Open **png2ico** folder in **Visual Studio**

## Windows with MSYS2

Install [MSYS2](https://www.msys2.org/) and follow instructions on **MSYS2** site to install the mingw-gcc toolchain

Open **MSYS2** console and install cmake
```bash
pacman -S cmake
```

Compile:
```bash
cd png2ico
mkdir build
cd build
cmake ..
mingw32-make
```

## Usage

```bash
png2ico ico_file.ico png_file_01.png [png_file_02.png ...]
```

## References
[ICO file](https://en.wikipedia.org/wiki/ICO_(file_format))
