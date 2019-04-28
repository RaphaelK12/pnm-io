// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "catch2/catch.hpp"
#include "catch_utils.h"
#include "thinks/pnm_io/pnm_io.h"

namespace {

void WriteInvalidPgmImage(std::ostream& os, std::string const& magic_number,
                          std::uint32_t const max_value,
                          std::size_t const width, std::size_t const height,
                          std::vector<std::uint8_t> const& pixel_data) {
  // Write header.
  os << magic_number << "\n"
     << width << "\n"
     << height << "\n"
     << max_value << "\n";  // Marks beginning of pixel data.

  // Write pixel data.
  os.write(reinterpret_cast<char const*>(pixel_data.data()), pixel_data.size());
}

std::vector<std::uint8_t> ValidPixelData(std::size_t const width,
                                         std::size_t const height) {
  return std::vector<std::uint8_t>(width * height);
}

}  // namespace

TEST_CASE("PGM - Write invalid filename throws") {
  auto constexpr width = std::size_t{10};
  auto constexpr height = std::size_t{10};
  auto const pixel_data = std::vector<std::uint8_t>(width * height);
  auto const filename = std::string{};  // Invalid.

  // Not checking error message since it is OS dependent.
  REQUIRE_THROWS_AS(
      thinks::WritePgmImage(filename, width, height, pixel_data.data()),
      std::runtime_error);
}

TEST_CASE("PGM - Write invalid width throws") {
  auto constexpr width = std::size_t{0};  // Invalid.
  auto constexpr height = std::size_t{10};
  auto const pixel_data = ValidPixelData(width, height);
  auto oss = std::ostringstream{};
  REQUIRE_THROWS_MATCHES(
      thinks::WritePgmImage(oss, width, height, pixel_data.data()),
      std::invalid_argument,
      ExceptionContentMatcher("width must be non-zero"));
}

TEST_CASE("PGM - Write invalid height throws") {
  auto constexpr width = std::size_t{10};
  auto constexpr height = std::size_t{0};  // Invalid.
  auto const pixel_data = ValidPixelData(width, height);
  auto oss = std::ostringstream{};
  REQUIRE_THROWS_MATCHES(
      thinks::WritePgmImage(oss, width, height, pixel_data.data()),
      std::invalid_argument,
      ExceptionContentMatcher("height must be non-zero"));
}

TEST_CASE("PGM - Read invalid filename throws") {
  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = ValidPixelData(width, height);
  auto const filename = std::string{};  // Invalid.

  // Not checking error message since it is OS dependent.
  REQUIRE_THROWS_AS(
      thinks::ReadPgmImage(filename, &width, &height, &pixel_data),
      std::runtime_error);
}

TEST_CASE("PGM - Read invalid magic number throws") {
  auto ss = std::stringstream{};
  WriteInvalidPgmImage(ss,
                       "P4",  // Invalid.
                       255, 10, 10, ValidPixelData(10, 10));

  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = std::vector<std::uint8_t>{};
  REQUIRE_THROWS_MATCHES(
      thinks::ReadPgmImage(ss, &width, &height, &pixel_data),
      std::runtime_error,
      ExceptionContentMatcher("magic number must be 'P5', was 'P4'"));
}

TEST_CASE("PGM - Read invalid width throws") {
  auto ss = std::stringstream{};
  WriteInvalidPgmImage(ss, "P5", 255,
                       0,  // Invalid.
                       10, ValidPixelData(0, 10));

  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = std::vector<std::uint8_t>{};
  REQUIRE_THROWS_MATCHES(
      thinks::ReadPgmImage(ss, &width, &height, &pixel_data),
      std::runtime_error,
      ExceptionContentMatcher("width must be non-zero"));
}

TEST_CASE("PGM - Read invalid height throws") {
  auto ss = std::stringstream{};
  WriteInvalidPgmImage(ss, "P5", 255, 10,
                       0,  // Invalid.
                       ValidPixelData(10, 0));

  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = std::vector<std::uint8_t>{};
  REQUIRE_THROWS_MATCHES(
      thinks::ReadPgmImage(ss, &width, &height, &pixel_data),
      std::runtime_error,
      ExceptionContentMatcher("height must be non-zero"));
}

TEST_CASE("PGM - Read invalid max value throws") {
  auto ss = std::stringstream{};
  WriteInvalidPgmImage(ss, "P5",
                       254,  // Invalid.
                       10, 10, ValidPixelData(10, 10));

  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = std::vector<std::uint8_t>{};
  REQUIRE_THROWS_MATCHES(
      thinks::ReadPgmImage(ss, &width, &height, &pixel_data),
      std::runtime_error,
      ExceptionContentMatcher("max value must be 255, was 254"));
}

TEST_CASE("PGM - Read invalid file size throws") {
  auto ss = std::stringstream{};
  WriteInvalidPgmImage(ss, "P5", 255, 10, 10,
                       ValidPixelData(10, 10 - 1));  // Invalid.

  auto width = std::size_t{0};
  auto height = std::size_t{0};
  auto pixel_data = std::vector<std::uint8_t>{};
  REQUIRE_THROWS_MATCHES(
      thinks::ReadPgmImage(ss, &width, &height, &pixel_data),
      std::runtime_error,
      ExceptionContentMatcher("failed reading 100 bytes"));
}

TEST_CASE("PGM - Round-trip") {
  auto constexpr write_width = std::size_t{16};
  auto constexpr write_height = std::size_t{16};
  auto constexpr write_pixel_count = write_width * write_height;
  auto write_pixels = std::vector<std::uint8_t>(write_pixel_count);
  for (auto i = std::size_t{0}; i < write_pixel_count; ++i) {
    write_pixels[i] = static_cast<std::uint8_t>(i);
  }

  // Write image to IO stream.
  auto ss = std::stringstream{};
  thinks::WritePgmImage(ss, write_width, write_height, write_pixels.data());

  // Read image from IO stream.
  auto read_width = std::size_t{0};
  auto read_height = std::size_t{0};
  auto read_pixels = std::vector<std::uint8_t>{};
  thinks::ReadPgmImage(ss, &read_width, &read_height, &read_pixels);

  // Check that values were preserved.
  REQUIRE(read_width == write_width);
  REQUIRE(read_height == write_height);
  REQUIRE(read_pixels == write_pixels);
}
