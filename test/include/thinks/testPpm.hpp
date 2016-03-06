#ifndef THINKS_PPM_TESTPPM_HPP_INCLUDED
#define THINKS_PPM_TESTPPM_HPP_INCLUDED

#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <thinks/ppm.hpp>

namespace thinks {
namespace ppm {
namespace test {

namespace detail {
} // namespace detail

inline
bool testWriteReadRoundTrip()
{
  using namespace std;

  auto const write_width = size_t{64};
  auto const write_height = size_t{96};
  auto write_pixels = vector<uint8_t>(write_width * write_height * 3);
  auto pixel_index = size_t{0};
  for (auto i = size_t{0}; i < write_height; ++i) {
    for (auto j = size_t{0}; j < write_width; ++j) {
      write_pixels[pixel_index * 3 + 0] = static_cast<uint8_t>(i);
      write_pixels[pixel_index * 3 + 1] = static_cast<uint8_t>(j);
      write_pixels[pixel_index * 3 + 2] = static_cast<uint8_t>(i + j);
      ++pixel_index;
    }
  }

  auto const filename = string("./test.ppm");

  writeRgb(filename, write_width, write_height, write_pixels);

  auto read_width = size_t{0};
  auto read_height = size_t{0};
  auto read_pixels = vector<uint8_t>();
  readRgb(filename, &read_width, &read_height, &read_pixels);

  auto passed = true;
  passed = passed && (read_width == write_width);
  passed = passed && (read_height == write_height);
  passed = passed && (read_pixels == write_pixels);

  return passed;
}

inline
bool testWriteInvalidFilename()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string("/invalid/file/name.ppm");
  auto const width = size_t{10};
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  try {
    writeRgb(filename, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool testWriteInvalidWidth()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string("./filename.ppm");
  auto const width = size_t{0};
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  try {
    writeRgb(filename, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool testWriteInvalidHeight()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string("./filename.ppm");
  auto const width = size_t{10};
  auto const height = size_t{0};
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  try {
    writeRgb(filename, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool testWriteInvalidPixelData()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string("./filename.ppm");
  auto const width = size_t{10};
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3 - 1);
  try {
    writeRgb(filename, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool testReadInvalidFilename()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string("/invalid/file/name.ppm");
  auto width = size_t{0};
  auto height = size_t{0};
  auto pixel_data = vector<uint8_t>();
  try {
    readRgb(filename, &width, &height, &pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool testReadInvalidMagicNumber()
{
  using namespace std;

  auto const filename = string("./invalid_magic_number.ppm");
  auto ofs = ofstream(filename, ios::binary);

  // Write header.
  auto const magic_number = string("P5");
  auto const max_value = string("255");
  ofs << magic_number << "\n"
      << 10 << " " << 10 << "\n"
      << max_value
      << "\n"; // Marks beginning of pixel data.

  // Write pixel data.
  auto const write_pixel_data = vector<uint8_t>(10 * 10 * 3);
  ofs.write(reinterpret_cast<char const*>(write_pixel_data.data()),
            write_pixel_data.size());

  ofs.close();

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgb(filename, &width, &height, &read_pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }

  return thrown;
}

inline
bool testReadInvalidMaxValue()
{
  using namespace std;

  auto const filename = string("./invalid_max_value.ppm");
  auto ofs = ofstream(filename, ios::binary);

  // Write header.
  auto const magic_number = string("P6");
  auto const max_value = string("254");
  ofs << magic_number << "\n"
      << 10 << " " << 10 << "\n"
      << max_value
      << "\n"; // Marks beginning of pixel data.

  // Write pixel data.
  auto const write_pixel_data = vector<uint8_t>(10 * 10 * 3);
  ofs.write(reinterpret_cast<char const*>(write_pixel_data.data()),
            write_pixel_data.size());

  ofs.close();

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgb(filename, &width, &height, &read_pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }

  return thrown;
}

inline
bool testReadInvalidFileSize()
{
  using namespace std;

  auto const filename = string("./invalid_magic_number.ppm");
  auto ofs = ofstream(filename, ios::binary);

  // Write header.
  auto const magic_number = string("P6");
  auto const max_value = string("255");
  ofs << magic_number << "\n"
      << 10 << " " << 10 << "\n"
      << max_value
      << "\n"; // Marks beginning of pixel data.

  // Write pixel data.
  auto const write_pixel_data = vector<uint8_t>(10 * 10 * 3 - 1);
  ofs.write(reinterpret_cast<char const*>(write_pixel_data.data()),
            write_pixel_data.size());

  ofs.close();

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgb(filename, &width, &height, &read_pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }

  return thrown;
}

} // namespace test
} // namespace ppm
} // namespace thinks

#endif // THINKS_PPM_TESTPPM_HPP_INCLUDED
