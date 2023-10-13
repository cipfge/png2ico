# png2ico

Create Windows icon file from a list of PNG images.

Images are be stored in the PNG format inside the ico file.

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
