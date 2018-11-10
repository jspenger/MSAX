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
//
// This is a modified (see below) version of the SAX algorithm proposed in:
//  Lin, Jessica, et al. "A symbolic representation of time series, with
//  implications for streaming algorithms." Proceedings of the 8th ACM SIGMOD
//  workshop on Research issues in data mining and knowledge discovery.  ACM,
//  2003.
//
// This implementation (MSAX) differs from the original SAX in two aspects.
//  (1) The time series is normalized by normalizing each point to the
//      neighbouring #windowsize points. That is, for each point we subtract
//      the moving average and divide by the moving standard deviation (with
//      a moving window of size windowsize). In the original SAX algorithm,
//      each subsequence is normalized.
//  (2) Because the entire time series is normalized, this implementation
//      outputs just one symbolic sequence, that represents the entire time
//      series. In comparison to the original SAX, which outputs a symbolic
//      sequence for every subsequence of the time series (of size windowsize),
//      resulting in more than one symbolic sequences.
//
//  To use this include file and run msax::run() with following parameters:
//    timeSeries: the time series (a container / vector)
//    alphabetSize: the size of the alphabet
//    frameSize: the size of a frame that gets transformed to one symbol, the
//               dimensionality reduction so to speak
//    windowSize: the size of the moving window used for the normalization
//
////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <boost/math/distributions/normal.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/rolling_variance.hpp>
#include <vector>

namespace msax {

template <typename T>
std::vector<int> run(const T& timeSeries,
                     size_t alphabetSize,
                     size_t frameSize,
                     size_t windowSize) {


  //////////////////////////////////////////////////////////////////////////////
  // Normalize the time series
  //////////////////////////////////////////////////////////////////////////////

  T normalizedTimeSeries(timeSeries.size());

  // accumulator for calculating moving variance and moving average
  boost::accumulators::accumulator_set
    <double,
    boost::accumulators::stats<boost::accumulators::tag::rolling_mean,
    boost::accumulators::tag::rolling_variance> >
    accumulator(boost::accumulators::tag::rolling_window::window_size = windowSize);

  // calculate the mean and variance for first windowsize elements
  for (size_t i = 0; i < windowSize; ++i) {
      accumulator(timeSeries[i]);
  }
  // calculate the normalized values for the first windowsize / 2 elements
  for (size_t i = 0; i < windowSize / 2; ++i) {
    normalizedTimeSeries[i] = (timeSeries[i] - boost::accumulators::rolling_mean(accumulator))
      / std::sqrt(boost::accumulators::rolling_variance(accumulator));
  }
  // calculate normalized values for windowsize/2 to timeSeries.size - (windowSize + 1) / 2 values
  for (size_t i = windowSize; i < timeSeries.size(); ++i) {
    accumulator(timeSeries[i]);
    size_t j = i - (windowSize + 1) / 2;
    normalizedTimeSeries[j] = (timeSeries[j] - boost::accumulators::rolling_mean(accumulator))
      / std::sqrt(boost::accumulators::rolling_variance(accumulator));
  }
  // calculate normalized values for the last (windowSize + 1) / 2 values
  for (size_t i = timeSeries.size() - (windowSize + 1) / 2; i < timeSeries.size(); ++i) {
    normalizedTimeSeries[i] = (timeSeries[i] - boost::accumulators::rolling_mean(accumulator))
      / std::sqrt(boost::accumulators::rolling_variance(accumulator));
  }


  //////////////////////////////////////////////////////////////////////////////
  // Perform PAA (Piecewise Aggregate Approximation)
  //////////////////////////////////////////////////////////////////////////////

  T paaTimeSeries((normalizedTimeSeries.size() - 1) / frameSize + 1);

  for (size_t i = 0; i < paaTimeSeries.size(); ++i) {
    double mean = 0.0;

    if (i == paaTimeSeries.size() - 1) {
      for (size_t j = i * frameSize; j < normalizedTimeSeries.size(); ++j) {
        mean += normalizedTimeSeries[j];
      }
      mean = mean / ((double)(normalizedTimeSeries.size() - i * frameSize));
    } else {
      for (size_t j = i * frameSize; j < (i+1) * frameSize; ++j) {
        mean += normalizedTimeSeries[j];
      }
      mean = mean / ((double)frameSize);
    }
    paaTimeSeries[i] = mean;
  }


  //////////////////////////////////////////////////////////////////////////////
  // Calculate breakpoints
  //////////////////////////////////////////////////////////////////////////////

  std::vector<double> breakpoints(alphabetSize - 1);
  boost::math::normal stdNormDist(0.0, 1.0); // standard normal distribution
  for (size_t i = 1; i < alphabetSize; ++i) {
    double q = quantile(stdNormDist, ((double) i) / ((double) alphabetSize));
    breakpoints[i-1] = q;
  }


  //////////////////////////////////////////////////////////////////////////////
  // Discretize
  //////////////////////////////////////////////////////////////////////////////

  std::vector<int> output(paaTimeSeries.size());
  // for each element of paaTimeSeries, map it to the corresponding symbol
  // as defined by the breakpoints, if breakpoints[i] < value < breakpoints[i+1],
  // then map it to symbol nom i+1
  for (size_t i = 0; i < output.size(); ++i) {
    for (size_t j = 0; j < breakpoints.size(); ++j) {
      output[i] = breakpoints.size();
      if (paaTimeSeries[i] < breakpoints[j]) {
        output[i] = j;
        break;
      }
    }
  }

  return output;
} // run()

} // namespace msax
