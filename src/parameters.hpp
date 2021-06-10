#pragma once

#include <random>

// population params
const int popSize = 1000; // not too many
const int generations = 1000;
const int timePerGen = 100;

// costs
const float costMove = 0.01f;
const float costSensing = 0.01f;
const float costCompete = 0.001f;

// landscape params
const float landsize = 100.f;

// mutation params
const float mutProb = 0.01f;
const float mutSize = 0.001f;

// make a gsl rng and a regular rng
extern std::mt19937 rng;

