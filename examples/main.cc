// Copyright(C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <pgm_example.h>
#include <ppm_example.h>
#include <simple_example.h>

int main(int argc, char* argv[]) {
  examples::PgmExample("./pgm_example.pgm");
  examples::PpmExample("./ppm_example.ppm");
  examples::SimpleExample("./simple.pgm", "./simple.ppm");

  return 0;
}
