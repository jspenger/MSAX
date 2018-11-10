////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2017 Jonas Spenger
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////


#include "msax.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>


int main(int argc, char const ** argv) {


  //////////////////////////////////////////////////////////////////////////////
  // Handle user input and implement help method.
  //////////////////////////////////////////////////////////////////////////////

  // Setup command line options parser.
  // For more information about the options parser (code copied and adapted from):
  // http://www.boost.org/doc/libs/1_65_0/doc/html/program_options/tutorial.html
  namespace po = boost::program_options;
  po::options_description desc("Options");

  // Define options.
  int windowSize;
  int alphabetSize;
  int frameSize;
  std::string mode;
  std::string filename;
  po::positional_options_description p;
  p.add("input", -1);
  desc.add_options()
    ("help,h", "Display help message.")
    ("windowsize,w", po::value<int>(&windowSize)->default_value(100),
      "Choose the sliding window size.")
    ("alphabetsize,a", po::value<int>(&alphabetSize)->default_value(8),
      "Select the size of the alphabet used for the lowdimensional approximation.")
    ("framesize,f", po::value<int>(&frameSize)->default_value(10),
      "Select the frame size of the dimensionality reduction, i.e. the amount of time series data points per symbol.")
    ("mode,m", po::value<std::string>(&mode)->default_value("normal"),
      "(normal | silent) Select the mode of the output.")
    ("input,i", po::value<std::string>(&filename),
      "The name of the input file from which to load the data")
    ("stream,s", "Read data from the standard input stream instead");

  // Parse command line options.
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  // Validate and parse option values.
  // Display help.
  if (vm.count("help")) {
    std::cout << "This is a simple c++ implementation of MSAX" << std::endl << std::endl;
    std::cout << "USAGE: MSAX (FILENAME | -i FILENAME | -s) [-w INT] [-a INT] [-n INT] [-m (normal | silent)]" <<
      std::endl << std::endl;
    std::cout << desc << "\n";
    return 0;
  }

  // Check if both filename and inputstream have been selected.
  if (vm.count("input") && vm.count("stream")) { // cant have both
    std::vector<std::string> ambigous_options;
    boost::throw_exception(po::ambiguous_option(std::vector<std::string>{"input", "stream"}));
  }

  // Check if neither filename nor inputstream have been selected.
  if (!vm.count("input") && !vm.count("stream")) { // must have either
    boost::throw_exception(po::required_option("input"));
  }

  // Check if file filename cannot be openend.
  std::ifstream infile;
  if (vm.count("input")) {
    filename = vm["input"].as< std::string>();
    infile.open(filename);
    if (!infile.is_open()) { // if could not open file
      throw po::validation_error(po::validation_error::invalid_option_value,
        "input", filename);
    }
  }

  // Check if sliding window size is smaller than 1.
  if (vm.count("windowsize")) {
    if (windowSize < 1)
      throw po::validation_error(po::validation_error::invalid_option_value,
        "windowsize", std::to_string(windowSize));
  }

  // Check if alphabet size is smaller than 1.
  if (vm.count("alphabetsize")) {
    if (alphabetSize < 1)
      throw po::validation_error(po::validation_error::invalid_option_value,
        "alphabetsize", std::to_string(alphabetSize));
  }

  // Check if number of symbols for the approximation is smaller than 1.
  if (vm.count("f")) {
    if (frameSize < 1)
      throw po::validation_error(po::validation_error::invalid_option_value,
        "f", std::to_string(frameSize));
  }

  // Check if mode is neither of the allowed values.
  if (vm.count("mode")) {
    if (mode != "silent" && mode != "normal")
      throw po::validation_error(po::validation_error::invalid_option_value,
        "mode", mode);
  }


  //////////////////////////////////////////////////////////////////////////////
  // Read file
  //////////////////////////////////////////////////////////////////////////////

  std::vector<double> input;

  // Load file infile to vector of doubles
  double v;
  if (vm.count("input")) {
    while(infile >> v) { // for every double
      input.push_back(v); // append double to input
    }
  } else if (vm.count("stream")) {
    while(std::cin >> v) { // for every double
      input.push_back(v); // append double to input
    }
  }


  //////////////////////////////////////////////////////////////////////////////
  // Perform MSAX
  //////////////////////////////////////////////////////////////////////////////

  std::vector<int> output = msax::run(input, alphabetSize, frameSize, windowSize);


  //////////////////////////////////////////////////////////////////////////////
  // Write output to cout
  //////////////////////////////////////////////////////////////////////////////

  if (mode != "silent") {
    for (int c : output) {
      std::cout << static_cast<char>('a' + c);
    }
  }

  return 0;
}
