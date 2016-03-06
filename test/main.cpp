#include <cstdlib>
#include <exception>
#include <iostream>

#include <thinks/testPpm.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
  using namespace std;
  using namespace thinks::ppm::test;

  auto success = true;
  try {
    // Write.
    success = success && testWriteInvalidFilename();
    success = success && testWriteInvalidWidth();
    success = success && testWriteInvalidHeight();
    success = success && testWriteInvalidPixelData();

    // Read.
    success = success && testReadInvalidFilename();
    success = success && testReadInvalidMagicNumber();
    success = success && testReadInvalidMaxValue();
    success = success && testReadInvalidFileSize();

    // Round trip.
    success = success && testWriteReadRoundTrip();
  }
  catch (std::exception& ex) {
    cerr << "Uncaught exception: " << ex.what() << endl;
  }

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
