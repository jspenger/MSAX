# MSAX

Time series discretization algorithm. This is a modified (see below) version of the SAX algorithm proposed in:
> Lin, Jessica, et al. "A symbolic representation of time series, with implications for streaming algorithms." Proceedings of the 8th ACM SIGMOD workshop on Research issues in data mining and knowledge discovery.  ACM, 2003.

This implementation (MSAX) differs from the original SAX in two aspects.

1. The time series is normalized by normalizing each point to the neighbouring #windowsize points. That is, for each point we subtract the moving average and divide by the moving standard deviation (with  a moving window of size windowsize). In the original SAX algorithm, each subsequence is normalized.
2. Because the entire time series is normalized, this implementation outputs just one symbolic sequence, that represents the entire time  series. In comparison to the original SAX, which outputs a symbolic sequence for every subsequence of the time series (of size windowsize),  resulting in more than one symbolic sequences.

Visualization of the MSAX algorithm on the test/insect_b.txt timeseries (first 500 datapoints).
![MSAX visualization](msax.png?raw=true "MSAX visualization")

## Prerequisites

* Boost
* CMake

## Installing

```
cd build;
cmake -DCMAKE_BUILD_TYPE=Release ..;
make;
cd ..;
```
or
```
sh build.sh;
```

## Testing

Run test script:

```
sh test.sh;
```

## Examples

For more information:
```
./build/MSAX -h
```

## Author

* Jonas Spenger

## Acknowledgments

* The software was developed as part of the Bachelor's thesis in 2017 at Humboldt University of Berlin.
