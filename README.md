# some tinkering

GSL on Windows: Download `local323.zip` from http://www.stats.ox.ac.uk/pub/Rtools/libs.html and place contents of `local323/lib/x64` in `C:\local\lib`.

GSL works out of the box on Linux systems with `libgsl` installed.

In `R`, make sure the GSL path is known.

```r
Sys.setenv("LIB_GSL" = "C:/local323")
```
