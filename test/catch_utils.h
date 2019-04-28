// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include <exception>
#include <sstream>
#include <string>

#include "catch2/catch.hpp"

struct ExceptionContentMatcher : Catch::MatcherBase<std::exception> {
  ExceptionContentMatcher(const std::string& target) : target_(target) {}

  bool match(const std::exception& matchee) const override {
    return matchee.what() == target_;
  }

  std::string describe() const override {
    auto oss = std::ostringstream{};
    oss << "exception message is: '" << target_ << "'";
    return oss.str();
  }

 private:
  std::string target_;
};
