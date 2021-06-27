#pragma once
#include "landscape.hpp"
#include "agent.hpp"

#include <Rcpp.h>

Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax,
               const int timesteps,
               FastNoiseLite &noise,
               const float landsize,
               const float clamp,
               const float perception,
               const int directions,
               const float costMove,
               const float costCompete);

Rcpp::List run_noisewalker(const int popsize,
                const int genmax, 
                const int timesteps,
                const float perception,
                const int directions,
                const float costMove,
                const float costCompete,
                const int nOctaves, 
                const double frequency,
                const float landsize,
                const float clamp);
