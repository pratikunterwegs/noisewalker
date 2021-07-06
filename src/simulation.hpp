#pragma once
#include "landscape.hpp"
#include "agent.hpp"

#include <Rcpp.h>

Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax,
               const int timesteps,
               FastNoiseLite &noise,
               FastNoiseLite &risk,
               const float landsize,
               const float clamp,
               const float perception,
               const int directions,
               const float costMove,
               const float allow_compete,
               const bool allow_coop);

Rcpp::List run_noisewalker(const int popsize,
                const int genmax, 
                const int timesteps,
                const float perception,
                const int directions,
                const float costMove,
                const int freqRisk, 
                const double freqRes,
                const float landsize,
                const float clamp,
                const bool random_traits,
                const bool allow_compete,
                const bool allow_coop);
