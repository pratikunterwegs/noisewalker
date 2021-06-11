#pragma once

#include <random>

// mutation params
const float mutProb = 0.01f;
const float mutSize = 0.001f;

// make a gsl rng and a regular rng
extern std::mt19937 rng;

