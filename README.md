# Personality 2D

This is a small model tying together a number of interesting tools to get at a persistent question of the co-variation of two orthogonal but related 'animal personality' traits.

## Landscape

The spatially explicit resource landscape is modelled using `noise`, provided by [`FastNoiseLite`](https://github.com/Auburn/FastNoiseLite).

## Competition

Competition among animals is modelled as a cost of proximity to other animals, and inter-individual distances are calculated using [`boost::Rtree`s](https://www.boost.org/doc/libs/1_76_0/libs/geometry/doc/html/geometry/spatial_indexes.html).

## Fast-ish/er Bernoulli distributions

Animal movement and behaviour is probabilistic, with hudreds of thousands of calls to `std::bernoulli_distribution`, not to mention setting up this distribution. This is (hopefully) speeded up by using the [GNU Scientific Library random number distributions](https://www.gnu.org/software/gsl/doc/html/randist.html).

## Simulations launched from, and data returned to, `R`

The simulations are disguised as easy-to-use `R` functions, using `Rcpp` to obscure the scary `C++` code that lies beneath. The simulation data are also returned to `R`, as well known objects (lists and data.frames).

## Using Rcpp extensions for Windows compatibility

GSL and Boost are ridiculously difficult to use on Windows (coming from Linux) --- but `Rcpp` extension packages `BH` (Boost Headers) and `RcppGSL` allow for this to happen seamlessly.

### GSL on Windows

However, some manual tinkering is required to get GSL libraries on Windows: Download `local323.zip` from http://www.stats.ox.ac.uk/pub/Rtools/libs.html and place contents of `local323/lib/x64` in `C:\local\lib`.

GSL works out of the box on Linux systems with `libgsl` installed.

In `R`, make sure the GSL path is known by running:

```r
Sys.setenv("LIB_GSL" = "C:/local323")
```
