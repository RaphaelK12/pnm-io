# PPM IO
This repository implements reading and writing of images in the [PNM format](https://en.wikipedia.org/wiki/Netpbm_format). The PPM image format is extremely simple, making it ideal for small exploratory projects. The major benefit of this simplicity is that is it possible to implement the PPM file format without using external dependencies, such as compression libraries. All production code in this repository is implemented in a single [header file](https://github.com/thinks/ppm-io/blob/master/include/thinks/ppm_io/ppm_io.h), making it very simple to add to any existing project without having to set up additional linker rules. Also, the implementation uses only standard types and holds no state, meaning it should be fairly straight-forward to use the read and write functions. Detailed documentation is available in the source code.

All code in this repository is released under the [MIT license](https://en.wikipedia.org/wiki/MIT_License).

## Usage
The implementation supports both reading and writing of PPM images. We provide some brief usage examples here, also refer to the [tests](https://github.com/thinks/ppm-io/blob/master/test/ppm_io_test.cc) for further examples.

Reading an image is done as follows.
```cpp
auto width = std::size_t{ 0 };
auto height = std::size_t{ 0 };
auto pixel_data = std::vector<std::uint8_t>{};
auto ifs = std::ifstream("my_file.ppm", ios::binary);
thinks::ppm_io::ReadRgbImage(ifs, &width, &height, &pixel_data);
ifs.close();
```
The above version uses the stream interface. This interface is the most flexible, since it does not assume that the image is stored on disk. Also, this version is useful for testing since it allows the tests to run in memory avoiding file permission issues. However, since the image being stored on disk is probably the most likely scenario a convenience version is also available.
```cpp
auto width = std::size_t{ 0 };
auto height = std::size_t{ 0 };
auto pixel_data = std::vector<std::uint8_t>{};
thinks::ppm_io::ReadRgbImage("my_file.ppm", &width, &height, &pixel_data);
```

Writing image files is done in a similar fashion.
```cpp
// Write a 10x10 image where all pixels have the value (128, 128, 128).
auto const width = std::size_t{ 10 };
auto const height = std::size_t{ 10 };
auto pixel_data = std::vector<std::uint8_t>(width * height * 3, 128);
auto ofs = std::ofstream("my_file.ppm", ios::binary);
thinks::ppm_io::WriteRgbImage(ofs, width, height, pixel_data);
ofs.close();
```
Again, there is a convenience version for writing to disk.
```cpp
// Write a 10x10 image where all pixels have the value (128, 128, 128).
auto const width = std::size_t{ 10 };
auto const height = std::size_t{ 10 };
auto pixel_data = std::vector<std::uint8_t>(width * height * 3, 128);
thinks::ppm_io::WriteRgbImage("my_file.ppm", width, height, pixel_data);
```

## Tests
Tests are implemented in the [Catch2](https://github.com/catchorg/Catch2) framework.
