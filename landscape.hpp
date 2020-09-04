#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <libnoise/noise.h>
#include "parameters.hpp"

using namespace noise;

/// set rng seed using GSL

/// make landscape as perlin module
void initNoise(const int nOctaves, const double frequency) {
    module::Perlin landscape;
    landscape.SetOctaveCount(nOctaves);
    landscape.SetFrequency(frequency);
}

#endif // LANDSCAPE_H
