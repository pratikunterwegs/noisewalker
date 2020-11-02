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
               const int genStart,
               const int genmax, const int timesteps,
               FastNoiseLite noise,
               std::vector<std::string> data_path)
{
    for (int gen = genStart; gen < genmax + genStart; ++gen) {
        /*if (gen % (genmax / 10) == 0) {
            std::cout << "gen = " << gen << "\n";
        } */       
        for (int t = 0; t < timesteps; ++t) {
            // if gen has not changed then move and forage
            popMoveForage(pop, noise);
        }
        // print mean and sd of mass
        printSummaryMass(pop, gen, data_path);
        doReproduce(pop);
    }
}


//' Runs the noisewalker simulation.
//'
//' @description Run the noisewalker simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param genmax The maximum number of generations per simulation.
//' @param timesteps The number of timesteps per generation.
//' @param nOctaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param frequencyTransfer Frequency of noise of the new landscape. A double
//' value between 1.0 and 16.0.
//' @param newSrange The sensory range of the population.
//' @param rep The replicate number. Designed to be read from a data.frame.
//' result in noise that is closer to white noise.
//' @return Nothing. Writes an image to file.
// [[Rcpp::export]]
void run_noisewalker(const int genmax, const int timesteps,
                    const int nOctaves, const double frequency,
                    const double frequencyTransfer,
                    const double newSrange, const std::string rep) {
    
    // set the random number generation etc
    const gsl_rng_type * T;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);

    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

    // init pop
    std::vector<Agent> pop (popSize);
    // force sensory range
    forceSrange(pop, newSrange);
    // random position
    randomPosition(pop);
    // random weights
    popRandomWeights(pop);

    // make the ancestral landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(nOctaves);

    // make the transplanted landscape
    // landscape this is relatively uniform
    FastNoiseLite newNoise;
    newNoise.SetSeed(seed);
    newNoise.SetFrequency(frequencyTransfer);
    newNoise.SetFractalOctaves(nOctaves);

    // idenity outpath
    std::vector<std::string> thisOutpath = identifyOutpath(nOctaves, frequency, 
        frequencyTransfer, rep);

    // print outpath for test
    // std::cout << "data at " << thisOutpath[0] << thisOutpath[1] << "\n";

    // do evolution
    evolvePop(pop, 0, genmax, timesteps, noise, thisOutpath);
    
    // now transfer the population
    evolvePop(pop, genmax, genmax, timesteps, newNoise, thisOutpath);

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
