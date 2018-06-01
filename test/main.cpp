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

#include <cstdlib>
#include <exception>
#include <iostream>

#include <thinks/testPpm.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
  using namespace std;
  using namespace thinks::ppm;

  auto success = true;
  try {
    // Write.
    success = success && test::writeInvalidFilename();
    success = success && test::writeInvalidWidth();
    success = success && test::writeInvalidHeight();
    success = success && test::writeInvalidPixelData();

    // Read.
    success = success && test::readInvalidFilename();
    success = success && test::readInvalidMagicNumber();
    success = success && test::readInvalidMaxValue();
    success = success && test::readInvalidFileSize();

    // Round trip.
    success = success && test::writeReadRoundTrip();
  }
  catch (exception& ex) {
    cerr << "Uncaught exception: " << ex.what() << endl;
    success = false;
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
