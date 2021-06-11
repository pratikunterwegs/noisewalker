#pragma once

#include <Rcpp.h>

Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax,
               const int timesteps,
               FastNoiseLite noise,
               const float perception);

Rcpp::List run_noisewalker(
        const int popsize,
        const int genmax, 
        const int timesteps,
        const float perception,
        const int nOctaves, 
        const double frequency);
