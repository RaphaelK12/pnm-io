// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef THINKS_PPM_IO_PPM_IO_H_INCLUDED
#define THINKS_PPM_IO_PPM_IO_H_INCLUDED

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace thinks {
namespace ppm_io {
namespace detail {

template <typename FileStreamT>
void OpenFileStream(
  FileStreamT* const file_stream,
  std::string const& filename,
  std::ios_base::openmode const mode = std::ios::binary) 
{
  file_stream->open(filename, mode);
  if (!(*file_stream)) {
    constexpr auto kErrMsgLen = std::size_t{ 1024 };
    char err_msg[kErrMsgLen];
    strerror_s(err_msg, kErrMsgLen, errno);
    auto oss = std::ostringstream{};
    oss << "cannot open file '" << filename << "', "
      << "error: '" << err_msg << "'";
    throw std::runtime_error(oss.str());
  }
}

}  // namespace detail


/*!
Read a PPM image from an input stream.

Pre-conditions:
  - the PPM header does not contain any comments.
  - the PPM header width and height are non-zero.
  - the input pointers are non-null.
   
Pixel data is read as RGB triplets in row major order. For instance,
the pixel data for a 2x2 image is represented as follows:
   
        Column 0                           Column 1                           
      +----------------------------------+------------------------------------+
      |                                  |                                    |
Row 0 | RGB: {data[0], data[1], data[2]} | RGB: {data[3], data[4], data[5]}   |
      |                                  |                                    |
      +----------------------------------+------------------------------------+
      |                                  |                                    |
Row 1 | RGB: {data[6], data[7], data[8]} | RGB: {data[9], data[10], data[11]} |
      |                                  |                                    |
      +----------------------------------+------------------------------------+

An std::runtime_error is thrown if:
  - the magic number is not 'P6'.
  - the max value is not '255'.
  - the pixel data cannot be read.
*/
inline 
void ReadRgbImage(
  std::istream& is, 
  std::size_t* const width,
  std::size_t* const height,
  std::vector<std::uint8_t>* const pixel_data) 
{
  assert(width != nullptr);
  assert(height != nullptr);
  assert(pixel_data != nullptr);

  // Read header.
  auto const expected_magic_number = std::string{ "P6" };
  auto const expected_max_value = std::string{ "255" };
  auto magic_number = std::string{};
  auto max_value = std::string{};
  is >> magic_number >> *width >> *height >> max_value;

  assert(*width != 0);
  assert(*height != 0);

  if (magic_number != expected_magic_number) {
    auto oss = std::ostringstream{};
    oss << "magic number must be '" << expected_magic_number << "'";
    throw std::runtime_error(oss.str());
  }

  if (max_value != expected_max_value) {
    auto oss = std::ostringstream{};
    oss << "max value must be " << expected_max_value;
    throw std::runtime_error(oss.str());
  }

  // Skip ahead (an arbitrary number!) to the pixel data.
  is.ignore(256, '\n');

  // Read pixel data.
  pixel_data->resize((*width) * (*height) * 3);
  is.read(reinterpret_cast<char*>(pixel_data->data()), pixel_data->size());

  if (!is) {
    auto oss = std::ostringstream();
    oss << "failed reading " << pixel_data->size() << " bytes";
    throw std::runtime_error(oss.str());
  }
}

/*! 
See std::istream overload version.

Throws an std::runtime_error if file cannot be opened.
*/
inline 
void ReadRgbImage(
  std::string const& filename, 
  std::size_t* const width,
  std::size_t* const height,
  std::vector<std::uint8_t>* const pixel_data) 
{
  auto ifs = std::ifstream{};
  detail::OpenFileStream(&ifs, filename);
  ReadRgbImage(ifs, width, height, pixel_data);
  ifs.close();
}


/*!
Write a PPM image to an output stream.

Pixel data is given as RGB triplets in row major order. For instance,
the pixel data for a 2x2 image is represented as follows:

        Column 0                           Column 1                           
      +----------------------------------+------------------------------------+
      |                                  |                                    |
Row 0 | RGB: {data[0], data[1], data[2]} | RGB: {data[3], data[4], data[5]}   |
      |                                  |                                    |
      +----------------------------------+------------------------------------+
      |                                  |                                    |
Row 1 | RGB: {data[6], data[7], data[8]} | RGB: {data[9], data[10], data[11]} |
      |                                  |                                    |
      +----------------------------------+------------------------------------+

An std::invalid_argument is thrown if:
  - width or height is zero.
  - the size of the pixel data does not match the width and height.
*/
inline 
void WriteRgbImage(
  std::ostream& os,
  std::size_t const width,
  std::size_t const height,
  std::vector<std::uint8_t> const& pixel_data)
{
  if (width == 0) {
    throw std::invalid_argument("width must be non-zero");
  }

  if (height == 0) {
    throw std::invalid_argument("height must be non-zero");
  }

  if (pixel_data.size() != width * height * 3) {
    throw std::invalid_argument("pixel data must match width and height");
  }

  // Write header.
  auto const magic_number = std::string{ "P6" };
  auto const max_value = std::string{ "255" };
  os << magic_number << "\n"
     << width << " " << height << "\n"
     << max_value << "\n";  // Marks beginning of pixel data.

  // Write pixel data.
  os.write(reinterpret_cast<char const*>(pixel_data.data()), pixel_data.size());
}

/*! 
See std::ostream overload version above.

Throws an std::runtime_error if file cannot be opened.
*/
inline 
void WriteRgbImage(
  std::string const& filename, 
  std::size_t const width,
  std::size_t const height,
  std::vector<std::uint8_t> const& pixel_data) 
{
  auto ofs = std::ofstream{};
  detail::OpenFileStream(&ofs, filename);
  WriteRgbImage(ofs, width, height, pixel_data);
  ofs.close();
}

}  // namespace ppm_io
}  // namespace thinks

#endif // THINKS_PPM_IO_PPM_IO_H_INCLUDED
