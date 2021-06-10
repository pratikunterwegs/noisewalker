
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
               FastNoiseLite noise,
               const float perception)
{
    genData thisGenData;
    // loop through generations
    for (int gen = 0; gen < genmax; ++gen) {
        for (int t = 0; t < timesteps; ++t) {
            popMoveForageCompete(pop, noise, perception, 8, 0.f); // set manually
        }
        thisGenData.updateGenData(pop, gen);
        // reproduce once generation is done
        doReproduce(pop);
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
//' @param nOctaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @return A dataframe of evolved pop strategy count.
// [[Rcpp::export]]
Rcpp::List run_noisewalker(
        const int popsize,
        const int genmax, 
        const int timesteps,
        const float perception,
        const int nOctaves, 
        const double frequency) {
    
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
    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(nOctaves);
    
    // do evolution
    Rcpp::List thisData = evolvePop(pop, genmax, timesteps, noise, perception);

    return thisData;
}

