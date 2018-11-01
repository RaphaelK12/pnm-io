// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef THINKS_PNM_IO_PNM_IO_H_INCLUDED
#define THINKS_PNM_IO_PNM_IO_H_INCLUDED

#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace thinks {
namespace pnm_io {
namespace detail {

template <typename FileStreamT>
void OpenFileStream(FileStreamT* const file_stream, std::string const& filename,
                    std::ios_base::openmode const mode = std::ios::binary) {
  assert(file_stream != nullptr && "file stream is null");

  file_stream->open(filename, mode);
  if (!(*file_stream)) {
    constexpr auto kErrMsgLen = std::size_t{1024};
    char err_msg[kErrMsgLen];
    strerror_s(err_msg, kErrMsgLen, errno);
    auto oss = std::ostringstream{};
    oss << "cannot open file '" << filename << "', "
        << "error: '" << err_msg << "'";
    throw std::runtime_error(oss.str());
  }
}

template <typename ExceptionT>
void ThrowIfInvalidMagicNumber(std::string const& magic_number,
                               std::string const& expected_magic_number) {
  if (magic_number != expected_magic_number) {
    auto oss = std::ostringstream{};
    oss << "magic number must be '" << expected_magic_number << "', was '"
        << magic_number << "'";
    throw ExceptionT(oss.str());
  }
}

template <typename ExceptionT>
void ThrowIfInvalidWidth(std::size_t const width) {
  if (width == 0) {
    throw ExceptionT("width must be non-zero");
  }
}

template <typename ExceptionT>
void ThrowIfInvalidHeight(std::size_t const height) {
  if (height == 0) {
    throw ExceptionT("height must be non-zero");
  }
}

template <typename ExceptionT>
void ThrowIfInvalidMaxValue(std::uint32_t const max_value) {
  constexpr auto expected_max_value =
      std::uint32_t{std::numeric_limits<std::uint8_t>::max()};
  if (max_value != expected_max_value) {
    auto oss = std::ostringstream{};
    oss << "max value must be " << expected_max_value << ", was " << max_value;
    throw ExceptionT(oss.str());
  }
}

template <typename ExceptionT>
void ThrowIfInvalidPixelData(std::vector<std::uint8_t> const& pixel_data,
                             std::size_t const expected_size) {
  if (pixel_data.size() != expected_size) {
    throw ExceptionT("pixel data must match width and height");
  }
}

struct Header {
  std::string magic_number = "";
  std::size_t width = 0;
  std::size_t height = 0;
  std::uint32_t max_value = 255;
};

inline Header ReadHeader(std::istream& is) {
  auto header = Header{};
  is >> header.magic_number >> header.width >> header.height >>
      header.max_value;

  ThrowIfInvalidWidth<std::runtime_error>(header.width);
  ThrowIfInvalidHeight<std::runtime_error>(header.height);
  ThrowIfInvalidMaxValue<std::runtime_error>(header.max_value);

  // Skip ahead (an arbitrary number!) to the pixel data.
  is.ignore(256, '\n');

  return header;
}

inline void WriteHeader(std::ostream& os, Header const& header) {
  ThrowIfInvalidWidth<std::invalid_argument>(header.width);
  ThrowIfInvalidHeight<std::invalid_argument>(header.height);
  ThrowIfInvalidMaxValue<std::invalid_argument>(header.max_value);

  os << header.magic_number << "\n"
     << header.width << "\n"
     << header.height << "\n"
     << header.max_value << "\n";  // Marks beginning of pixel data.
}

inline void ReadPixelData(std::istream& is,
                          std::vector<std::uint8_t>* const pixel_data) {
  is.read(reinterpret_cast<char*>(pixel_data->data()), pixel_data->size());

  if (!is) {
    auto oss = std::ostringstream();
    oss << "failed reading " << pixel_data->size() << " bytes";
    throw std::runtime_error(oss.str());
  }
}

inline void WritePixelData(std::ostream& os,
                           std::uint8_t const* const pixel_data,
                           std::size_t const size) {
  os.write(reinterpret_cast<char const*>(pixel_data), size);
}

}  // namespace detail

/*!
Read a PGM (greyscale) image from an input stream.

Pre-conditions:
  - the PGM header does not contain any comments.
  - the output pointers are non-null.

Pixel data is read as RGB triplets in row major order. For instance,
the pixel data for a 2x2 image is represented as follows:

        Column 0      Column 1
      +-------------+-------------+
      |             |             |
Row 0 | I: data[0]  | I: data[1]  |
      |             |             |
      +-------------+-------------+
      |             |             |
Row 1 | I: data[2]  | I: data[3]  |
      |             |             |
      +-------------+-------------+

An std::runtime_error is thrown if:
  - the magic number is not 'P5'.
  - width or height is zero.
  - the max value is not '255'.
  - the pixel data cannot be read.
*/
inline void ReadPgmImage(std::istream& is, std::size_t* const width,
                         std::size_t* const height,
                         std::vector<std::uint8_t>* const pixel_data) {
  auto header = detail::ReadHeader(is);
  detail::ThrowIfInvalidMagicNumber<std::runtime_error>(header.magic_number,
                                                        "P5");

  assert(width != nullptr && "null width");
  assert(height != nullptr && "null height");
  *width = header.width;
  *height = header.height;

  assert(pixel_data != nullptr && "null pixel data");
  pixel_data->resize((*width) * (*height));
  detail::ReadPixelData(is, pixel_data);
}

/*!
See std::istream overload version.

Throws an std::runtime_error if file cannot be opened.
*/
inline void ReadPgmImage(std::string const& filename, std::size_t* const width,
                         std::size_t* const height,
                         std::vector<std::uint8_t>* const pixel_data) {
  auto ifs = std::ifstream{};
  detail::OpenFileStream(&ifs, filename);
  ReadPgmImage(ifs, width, height, pixel_data);
  ifs.close();
}

/*!
Write a PGM (greyscale) image to an output stream.

Pixel data is given a intensities in row major order. For instance,
the pixel data for a 2x2 image is represented as follows:

        Column 0      Column 1
      +-------------+-------------+
      |             |             |
Row 0 | I: data[0]  | I: data[1]  |
      |             |             |
      +-------------+-------------+
      |             |             |
Row 1 | I: data[2]  | I: data[3]  |
      |             |             |
      +-------------+-------------+

An std::invalid_argument is thrown if:
  - width or height is zero.
  - the size of the pixel data does not match the width and height.
*/
inline void WritePgmImage(std::ostream& os, std::size_t const width,
                          std::size_t const height,
                          std::uint8_t const* const pixel_data) {
  auto header = detail::Header{};
  header.magic_number = "P5";
  header.width = width;
  header.height = height;
  detail::WriteHeader(os, header);
  detail::WritePixelData(os, pixel_data, header.width * header.height);
}

/*!
See std::ostream overload version above.

Throws an std::runtime_error if file cannot be opened.
*/
inline void WritePgmImage(std::string const& filename, std::size_t const width,
                          std::size_t const height,
                          std::uint8_t const* const pixel_data) {
  auto ofs = std::ofstream{};
  detail::OpenFileStream(&ofs, filename);
  WritePgmImage(ofs, width, height, pixel_data);
  ofs.close();
}

/*!
Read a PPM (RGB) image from an input stream.

Pre-conditions:
  - the PPM header does not contain any comments.
  - the output pointers are non-null.

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
  - width or height is zero.
  - the max value is not '255'.
  - the pixel data cannot be read.
*/
inline void ReadPpmImage(std::istream& is, std::size_t* const width,
                         std::size_t* const height,
                         std::vector<std::uint8_t>* const pixel_data) {
  auto header = detail::ReadHeader(is);
  detail::ThrowIfInvalidMagicNumber<std::runtime_error>(header.magic_number,
                                                        "P6");

  assert(width != nullptr && "null width");
  assert(height != nullptr && "null height");
  *width = header.width;
  *height = header.height;

  assert(pixel_data != nullptr && "null pixel data");
  pixel_data->resize((*width) * (*height) * 3);
  detail::ReadPixelData(is, pixel_data);
}

/*!
See std::istream overload version.

Throws an std::runtime_error if file cannot be opened.
*/
inline void ReadPpmImage(std::string const& filename, std::size_t* const width,
                         std::size_t* const height,
                         std::vector<std::uint8_t>* const pixel_data) {
  auto ifs = std::ifstream{};
  detail::OpenFileStream(&ifs, filename);
  ReadPpmImage(ifs, width, height, pixel_data);
  ifs.close();
}

/*!
Write a PPM (RGB) image to an output stream.

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
inline void WritePpmImage(std::ostream& os, std::size_t const width,
                          std::size_t const height,
                          std::uint8_t const* const pixel_data) {
  auto header = detail::Header{};
  header.magic_number = "P6";
  header.width = width;
  header.height = height;
  detail::WriteHeader(os, header);
  detail::WritePixelData(os, pixel_data, header.width * header.height * 3);
}

/*!
See std::ostream overload version above.

Throws an std::runtime_error if file cannot be opened.
*/
inline void WritePpmImage(std::string const& filename, std::size_t const width,
                          std::size_t const height,
                          std::uint8_t const* const pixel_data) {
  auto ofs = std::ofstream{};
  detail::OpenFileStream(&ofs, filename);
  WritePpmImage(ofs, width, height, pixel_data);
  ofs.close();
}

}  // namespace pnm_io
}  // namespace thinks

#endif  // THINKS_PNM_IO_PNM_IO_H_INCLUDED
