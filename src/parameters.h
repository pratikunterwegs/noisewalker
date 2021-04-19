#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <random>

// population params
const int popSize = 1000; // not too many
const int generations = 1000;
const int timePerGen = 100;
const int maxTimesteps = timePerGen * generations; // a few generations

// movement cost
// const float move_cost = 0.001f;
const float mass_init = 10.f;
const float mass_min = 0.001f;
const double mass_move_ratio = 1.f;
const float mass_cost = 0.001f;

// landscape params
const float landsize = 100.f;
const float clamp = 0.5f;

// mutation params
const float mShift = 0.01f;
const float mProb = 0.01f;
const float mShiftA = 0.05f;

// init rng
std::mt19937 rng;

#endif // PARAMETERS_H
