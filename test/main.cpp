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
