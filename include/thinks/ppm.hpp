#ifndef THINKS_PPM_PPM_HPP_INCLUDED
#define THINKS_PPM_PPM_HPP_INCLUDED

#include <cassert>
#include <cstdint>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace thinks {
namespace ppm {

namespace detail {

template<typename F> inline
F openFileStream(
  std::string const& filename,
  std::ios_base::openmode const mode = std::ios::binary)
{
  using namespace std;

  auto ofs = F(filename, mode);
  if (ofs.fail()) {
    auto ss = stringstream();
    ss << "cannot open file '" << filename << "'";
    throw runtime_error(ss.str());
  }

  return ofs;
}

} // namespace detail

//! Read a PPM image from disk.
//!
//! Assumptions:
//! - the PPM file header does not contain any comments.
//! - the PPM file header width and height are non-zero.
//! - the input pointers are non-null.
//!
//! Pixel data is read as RGB triplets in row major order. For instance,
//! the first two pixels of the first row are read as:
//!
//! Row 0: (R: data[0], G: data[1], B: data[2]), (R: data[3], G: data[4], B: data[5])
//!
//! An std::runtime_error is thrown if:
//! - file cannot be opened.
//! - the magic number is not 'P6'.
//! - the max value is not '255'.
inline
void readRgb(
  std::string const& filename,
  std::size_t* width,
  std::size_t* height,
  std::vector<std::uint8_t>* pixel_data)
{
  using namespace std;
  using namespace detail;

  assert(width != nullptr);
  assert(height != nullptr);
  assert(pixel_data != nullptr);

  auto ifs = openFileStream<ifstream>(filename);

  // Read header.
  auto const expected_magic_number = string("P6");
  auto const expected_max_value = string("255");
  auto magic_number = string();
  auto max_value = string();
  ifs >> magic_number >> *width >> *height >> max_value;

  assert(*width != 0);
  assert(*height != 0);

  if (magic_number != expected_magic_number) {
    auto ss = stringstream();
    ss << "magic number must be '" << expected_magic_number << "'";
    throw runtime_error(ss.str());
  }

  if (max_value != expected_max_value) {
    auto ss = stringstream();
    ss << "max value must be " << expected_max_value;
    throw runtime_error(ss.str());
  }

  // Skip ahead (an arbitrary number!) to the pixel data.
  ifs.ignore(256, '\n');

  // Read pixel data.
  pixel_data->resize((*width) * (*height) * 3);
  ifs.read(reinterpret_cast<char*>(pixel_data->data()), pixel_data->size());

  if (!ifs) {
    auto ss = stringstream();
    ss << "failed reading " << pixel_data->size() << " bytes";
    ifs.close();
    throw runtime_error(ss.str());
  }

  ifs.close();
}


//! Write a PPM image to disk.
//! Pixel data is given as RGB triplets in row major order. For instance,
//! the first two pixels of the first row are given as:
//!
//! Row 0: (R: data[0], G: data[1], B: data[2]), (R: data[3], G: data[4], B: data[5])
//!
//! An std::runtime_error is thrown if:
//! - file cannot be opened.
//! - width or height is zero.
//! - the size of the pixel data does not match the width and height.
inline
void writeRgb(
  std::string const& filename,
  std::size_t const width,
  std::size_t const height,
  std::vector<std::uint8_t> const& pixel_data)
{
  using namespace std;
  using namespace detail;

  if (width == 0) {
    throw runtime_error("width must be non-zero");
  }

  if (height == 0) {
    throw runtime_error("height must be non-zero");
  }

  if (pixel_data.size() != width * height * 3) {
    throw runtime_error("pixel data must match width and height");
  }

  auto ofs = openFileStream<ofstream>(filename);

  // Write header.
  auto const magic_number = string("P6");
  auto const max_value = string("255");
  ofs << magic_number << "\n"
      << width << " " << height << "\n"
      << max_value
      << "\n"; // Marks beginning of pixel data.

  // Write pixel data.
  ofs.write(reinterpret_cast<char const*>(pixel_data.data()), pixel_data.size());

  ofs.close();
}

} // namespace ppm
} // namespace thinks

#endif // THINKS_PPM_PPM_HPP_INCLUDED
