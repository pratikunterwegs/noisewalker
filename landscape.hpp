#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <libnoise/noise.h>
#include "parameters.hpp"

using namespace noise;

/// set rng seed using GSL

/// make landscape as perlin module
module::Perlin makeNoise(const int nOctaves, const double frequency) {
    module::Perlin landscape;
    landscape.SetOctaveCount(nOctaves);
    landscape.SetFrequency(frequency);

    return landscape;
}

#endif // LANDSCAPE_H
