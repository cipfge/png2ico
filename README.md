# png2ico

Create Windows icon file from a list of PNG images.

Images are be stored in the PNG format inside the ico file with a small entry header (16 bytes).

## Requirements
- C++ compiler (gcc, clang, MSVC)
- CMake

## Compilation

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
png2ico icofile pngfile1 [pngfile2 ...]
```
