// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "pgm_example.h"

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

void PgmExample(const std::string& filename) {
  constexpr auto width = std::size_t{128};
  constexpr auto height = std::size_t{128};
  auto pixel_data = std::vector<std::uint8_t>(width * height, std::uint8_t{0});

  // Set pixel data to a circular gradient.
  constexpr auto channel_max = std::numeric_limits<std::uint8_t>::max();
  const auto row_max_pos = (height - 1) + 0.5f;
  const auto col_max_pos = (width - 1) + 0.5f;
  const auto max_dist =
      std::sqrt(row_max_pos * row_max_pos + col_max_pos * col_max_pos) -
      std::sqrt(0.5f * 0.5f + 0.5f * 0.5f);
  for (auto row = std::size_t{0}; row < height; ++row) {
    for (auto col = std::size_t{0}; col < width; ++col) {
      const auto col_pos = col + 0.5f;
      const auto row_pos = row + 0.5f;
      const auto dist = std::sqrt(row_pos * row_pos + col_pos * col_pos);
      const auto dist_ratio = clamp(0.f, 1.f, dist / max_dist);
      const auto i = static_cast<std::uint8_t>(dist_ratio * channel_max);
      const auto j = col + row * width;
      pixel_data[j] = i;
    }
  }

  thinks::WritePgmImage(filename, width, height, pixel_data.data());
}

}  // namespace examples
