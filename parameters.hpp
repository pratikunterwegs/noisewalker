#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <random>
// gsl libs
#include <stdio.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


// population params
const int popSize = 500; // not too many
const int genmax = 1000; // a few generations

// landscape params
const float landsize = 100.f;

// mutation params
const float mShift = 0.01f;
const float mProb = 0.01f;
const float mShiftA = 0.05f;

// make a gsl rng and a regular rng
gsl_rng * r ;
std::mt19937 rng;

#endif // PARAMETERS_H
