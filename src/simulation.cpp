#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include "parameters.h"
#include "agent.h"
#include "noisewalker_tools.h"
#include "FastNoiseLite.h"
#include <Rcpp.h>

using namespace Rcpp;

/// function to evolve population
void evolvePop(std::vector<Agent> &pop,
               const int genmax, const int timesteps,
               FastNoiseLite noise,
               const double landsize)
{
    for (int gen = 0; gen < genmax; ++gen) {
        for (int t = 0; t < timesteps; ++t) {
            // if gen has not changed then move and forage
            popMoveForage(pop, noise, landsize);
        }
        doReproduce(pop);
    }
}


//' Runs the noisewalker simulation.
//'
//' @description Run the noisewalker simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param popsize The population size.
//' @param genmax The maximum number of generations per simulation.
//' @param timesteps The number of timesteps per generation.
//' @param nOctaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param landsize The landscape size.
//' @return A dataframe of evolved pop strategy count.
// [[Rcpp::export]]
Rcpp::DataFrame run_noisewalker(
        const int popsize,
        const int genmax, const int timesteps,
        const int nOctaves, const double frequency,
        const double landsize) {
    
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

    rng(seed);
    
    // init pop
    std::vector<Agent> pop (popsize, Agent(1, 1.f));
    // random position
    popRandomPos(pop, landsize);
    // random weights
    popRandomWeights(pop);
    
    // make the ancestral landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(nOctaves);
    
    // do evolution
    evolvePop(pop, genmax, timesteps, noise, landsize);
    
    return Rcpp::DataFrame::create(
        Named("strategy") = std::vector<int> {1, 2, 3},
        Named("prop") = getPopStrategyProp(pop)
    );
}

// helper function to print values
//' Run a transect through 1D Perlin noise.
//'
//' @param nOctaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' @param increment The increment in the X coordinate.
//' @param nValues How many steps, each of \code{increment} magnitude, to take.
// [[Rcpp::export]]
Rcpp::NumericVector get_values_1d(const double frequency,
                                  const float increment, const int nValues) {
    // get seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    
    // make perlin noise
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(frequency);
    
    // make a vector to hold values
    Rcpp::NumericVector sampleVals (nValues);
    
    float coordX = static_cast<float> (seed) / static_cast<float> (1e8);
    float coordY = 0.f;
    for (size_t i = 0; i < nValues; ++i) {
        sampleVals[i] = noise.GetNoise(coordX, coordY);
        coordX += increment;
    }
    return sampleVals;
}

#endif // SIMULATION_H
