/// code to output Perlin noise landscapes
# include <vector>
# include <Rcpp.h>

Rcpp::NumericMatrix getLandscape(const float landsize, const int octaves,
	const float frequency, const float increments);
