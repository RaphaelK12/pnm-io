// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "ppm_example.h"

#include <limits>
#include <vector>

#include "thinks/pnm_io/pnm_io.h"

namespace {

template <typename T>
T clamp(const T min_value, const T max_value, const T value) {
  return value < min_value ? min_value
                           : (value > max_value ? max_value : value);
}

}  // namespace

namespace examples {

void PpmExample(const std::string& filename) {
  constexpr auto width = std::size_t{128};
  constexpr auto height = std::size_t{128};
  auto pixel_data =
      std::vector<std::uint8_t>(3 * width * height, std::uint8_t{0});

  // Set pixel data to a colorful gradient.
  // As we move further down the rows the image gets greener,
  // as we move further along the columns to the right the image gets
  // redder. Top left corner is blue-ish and bottom right corner is
  // yellow-ish.
  constexpr auto channel_max = std::numeric_limits<std::uint8_t>::max();
  constexpr auto row_step = 1.f / (height - 1);
  constexpr auto col_step = 1.f / (width - 1);
  for (auto row = std::size_t{0}; row < height; ++row) {
    for (auto col = std::size_t{0}; col < width; ++col) {
      const auto col_ratio = clamp(0.f, 1.f, (col + 0.5f) * col_step);
      const auto row_ratio = clamp(0.f, 1.f, (row + 0.5f) * row_step);
      const auto r = static_cast<std::uint8_t>(col_ratio * channel_max);
      const auto g = static_cast<std::uint8_t>(row_ratio * channel_max);
      const auto b = std::uint8_t{128};
      const auto i = 3 * (col + row * width);
      pixel_data[i + 0] = r;
      pixel_data[i + 1] = g;
      pixel_data[i + 2] = b;
    }
  }

  thinks::WritePpmImage(filename, width, height, pixel_data.data());
}

}  // namespace examples
