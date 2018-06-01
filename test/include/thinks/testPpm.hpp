// Copyright 2018 Tommy Hinks
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

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

void writeInvalidRgbImage(
  std::ostream& os,
  std::string const& magic_number,
  std::string const& max_value,
  std::size_t const width,
  std::size_t const height,
  std::vector<std::uint8_t> const& pixel_data)
{
  using namespace std;

  // Write header.
  os << magic_number << "\n"
     << width << " " << height << "\n"
     << max_value
     << "\n"; // Marks beginning of pixel data.

  // Write pixel data.
  os.write(reinterpret_cast<char const*>(pixel_data.data()), pixel_data.size());
}

} // namespace detail

inline
bool writeReadRoundTrip()
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

  // Write image to IO stream.
  auto ss = stringstream();
  writeRgbImage(ss, write_width, write_height, write_pixels);

  // Read image from IO stream.
  auto read_width = size_t{0};
  auto read_height = size_t{0};
  auto read_pixels = vector<uint8_t>();
  readRgbImage(ss, &read_width, &read_height, &read_pixels);

  // Check that values were preserved.
  auto passed = true;
  passed = passed && (read_width == write_width);
  passed = passed && (read_height == write_height);
  passed = passed && (read_pixels == write_pixels);

  return passed;
}

inline
bool writeInvalidFilename()
{
  using namespace std;

  auto thrown = false;
  auto const filename = string(); // Invalid.
  auto const width = size_t{10};
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  try {
    writeRgbImage(filename, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool writeInvalidWidth()
{
  using namespace std;

  auto ss = stringstream();
  auto const width = size_t{0}; // Invalid.
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  auto thrown = false;
  try {
    writeRgbImage(ss, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool writeInvalidHeight()
{
  using namespace std;

  auto ss = stringstream();
  auto const width = size_t{10};
  auto const height = size_t{0}; // Invalid.
  auto const pixel_data = vector<uint8_t>(width * height * 3);
  auto thrown = false;
  try {
    writeRgbImage(ss, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool writeInvalidPixelData()
{
  using namespace std;

  auto ss = stringstream();
  auto const width = size_t{10};
  auto const height = size_t{10};
  auto const pixel_data = vector<uint8_t>(width * height * 3 - 1); // Invalid.
  auto thrown = false;
  try {
    writeRgbImage(ss, width, height, pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool readInvalidFilename()
{
  using namespace std;

  auto const filename = string(""); // Invalid.
  auto width = size_t{0};
  auto height = size_t{0};
  auto pixel_data = vector<uint8_t>();
  auto thrown = false;
  try {
    readRgbImage(filename, &width, &height, &pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }
  return thrown;
}

inline
bool readInvalidMagicNumber()
{
  using namespace std;

  auto ss = stringstream();
  detail::writeInvalidRgbImage(
    ss,
    "P5", // Invalid.
    "255",
    10,
    10,
    vector<uint8_t>(10 * 10 * 3));

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgbImage(ss, &width, &height, &read_pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }

  return thrown;
}

inline
bool readInvalidMaxValue()
{
  using namespace std;

  auto ss = stringstream();
  detail::writeInvalidRgbImage(
    ss,
    "P6",
    "254", // Invalid.
    10,
    10,
    vector<uint8_t>(10 * 10 * 3));

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgbImage(ss, &width, &height, &read_pixel_data);
  }
  catch (runtime_error&) {
    thrown = true;
  }

  return thrown;
}

inline
bool readInvalidFileSize()
{
  using namespace std;

  auto ss = stringstream();
  detail::writeInvalidRgbImage(
    ss,
    "P6",
    "256",
    10,
    10,
    vector<uint8_t>(10 * 10 * 3 - 1)); // Invalid.

  auto thrown = false;
  try {
    auto width = size_t{0};
    auto height = size_t{0};
    auto read_pixel_data = vector<uint8_t>();
    readRgbImage(ss, &width, &height, &read_pixel_data);
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
