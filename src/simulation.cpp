
#include <iostream>
#include <chrono>
#include "parameters.hpp"
#include "agent.hpp"
#include "datatypes.hpp"
#include "simulation.hpp"
#include "FastNoiseLite.h"
#include <Rcpp.h>

using namespace Rcpp;

/// function to evolve population
Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax,
               const int timesteps,
               FastNoiseLite &noise,
               FastNoiseLite &risk,
               const float landsize,
               const float clamp,
               const float perception,
               const int directions,
               const float costMove)
{
    genData thisGenData;
    // loop through generations
    for (int gen = 0; gen < genmax; ++gen) {
        for (int t = 0; t < timesteps; ++t) {
            popMoveForageCompete(pop, noise, risk, t, perception, directions, landsize,
            clamp, costMove); // set manually
        }
        thisGenData.updateGenData(pop, gen);
        // reproduce once generation is done
        doReproduce(pop, landsize);
    }
    return thisGenData.returnGenData();
}


//' Runs the noisewalker simulation.
//'
//' @description Run the noisewalker simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param popsize The population size.
//' @param genmax The maximum number of generations per simulation.
//' @param timesteps The number of timesteps per generation.
//' @param perception The perception range.
//' @param directions The number of points at which agents sense resources,
//' at a fixed distance of \code{perception} units away from them.
//' @param costMove The cost per move; distance moved is assumed constant.
//' @param freqRisk The patchiness of the landscape of costs.
//' @param freqRes Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param landsize The size of the landscape after which movement is wrapped.
//' Actually, the landscape is infinite, and does not have a repeating pattern.
//' @param clamp The threshold value of the landscape below which, the agents
//' sense and receive zero resources. Needed because noise has values -1 to +1.
//' @return A dataframe of evolved pop strategy count.
// [[Rcpp::export]]
Rcpp::List run_noisewalker(
        const int popsize,
        const int genmax, 
        const int timesteps,
        const float perception,
        const int directions,
        const float costMove,
        const float freqRisk, 
        const double freqRes,
        const float landsize,
        const float clamp) {
    
    // set up seed etc
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);
    
    // init pop
    std::vector<Agent> pop (popsize);
    // random position
    popRandomPos(pop, landsize); // landsize is fixed in parameters.hpp
    // random weights
    popRandomTraits(pop);
    
    // make the ancestral landscape
    FastNoiseLite noise;
    FastNoiseLite risk;
    noise.SetSeed(seed);
    risk.SetSeed(seed + 1);
    noise.SetFrequency(freqRes);
    risk.SetFrequency(freqRisk);
    
    // do evolution
    Rcpp::List thisData = evolvePop(pop, genmax, timesteps, noise,
                                    risk, landsize, clamp, perception,
                                    directions, costMove);

    return thisData;
}

