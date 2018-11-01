// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <simple_example.h>

#include <vector>

#include <thinks/pnm_io/pnm_io.h>

namespace {

void WritePgmImage(const std::string& filename) {
  constexpr auto width = std::size_t{10};
  constexpr auto height = std::size_t{10};
  auto pixel_data = std::vector<std::uint8_t>(width * height, 128);
  thinks::pnm_io::WritePgmImage(filename, width, height, pixel_data.data());
}

void WritePpmImage(const std::string& filename) {
  constexpr auto width = std::size_t{10};
  constexpr auto height = std::size_t{10};
  auto pixel_data = std::vector<std::uint8_t>(width * height * 3, 128);
  thinks::pnm_io::WritePpmImage(filename, width, height, pixel_data.data());
}

}  // namespace

namespace examples {

void SimpleExample(const std::string& pgm_filename,
                   const std::string& ppm_filename) {
  WritePgmImage(pgm_filename);
  WritePpmImage(ppm_filename);
}

}  // namespace examples
