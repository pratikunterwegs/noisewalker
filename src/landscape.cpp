/// definition of landscape related functions

#include <chrono>
#include <cmath>
#include "landscape.hpp"
#include "FastNoiseLite.h"

//' Get samples from the Perlin noise landscape.
//'
//' @description Returns a matrix of landscape values.
//'
//' @param landsize The landscape size.
//' @param octaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param increment The distance between samples. Landscape variabiltiy is scale dependent,
//' so sampling distane must be the same when comparing between landscapes.
//' @param clamp The landscape value below which to return 0, useful when landscape has negative values.
// [[Rcpp::export]]
Rcpp::NumericMatrix getLandscape(const float landsize, const int octaves,
	const float frequency, const float increment, const float clamp) {

	// set up seed etc
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

    // make landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(octaves);

    // calculate how many points for landsize / increments
    int nrow = static_cast<int> (std::floor(landsize / increment));

    // make square matrix of size nrow
    Rcpp::NumericMatrix samples(nrow);

    // loop through n samples
    float x = 0.f; float y = 0.f;
    for (int i = 0; i < nrow; ++i)
    {	
    	// reset y
    	y = 0.f;
    	for (int j = 0; j < nrow; ++j)
    	{
    		float val = noise.GetNoise(x, y);
    		if (val < clamp)
    		{
    			samples (i, j) = 0.f;
    		}
    		else samples (i, j) = val;
    		y += increment;
    	}
    	x += increment;
    }
    // return the filled matrix
    return samples;
}
