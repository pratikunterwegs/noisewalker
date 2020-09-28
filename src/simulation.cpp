#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include "noise/noise.h"
#include "noiseutils.h"
#include "parameters.h"
#include "agent.h"
#include "noisewalker_tools.h"
#include <Rcpp.h>

using namespace Rcpp;

/// function to evolve population
void evolvePop(std::vector<Agent> &pop,
               const int genmax, const int timesteps,
               module::Perlin noise)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    for (int gen = 0; gen < genmax; ++gen) {
        /*if (gen % (genmax / 10) == 0) {
            std::cout << "gen = " << gen << "\n";
        } */       
        for (int t = 0; t < timesteps; ++t) {
            // if gen has not changed then move and forage
            popMoveForage(pop, noise);
        }
        doReproduce(pop);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    // std::cout << "pop evolve time: " << duration << "\n";

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
//' @param newSrange The sensory range of the population.
//' @param rep The replicate number. Designed to be read from a data.frame.
//' result in noise that is closer to white noise.
//' @return Nothing. Writes an image to file.
// [[Rcpp::export]]
void run_noisewalker(const int genmax, const int timesteps,
                    const int nOctaves, const double frequency,
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

    // landscape this is relatively uniform
    module::Perlin noise;
    noise.SetOctaveCount(nOctaves);
    noise.SetFrequency(frequency);
    noise.SetPersistence(0.5);
    noise.SetSeed(static_cast<int> (seed));

    // idenity outpath
    std::vector<std::string> thisOutpath = identifyOutpath(nOctaves, frequency, rep);

    // print outpath for test
    // std::cout << "data at " << thisOutpath[0] << thisOutpath[1] << "\n";

    // do evolution
    evolvePop(pop, genmax, timesteps, noise);
    printPopMass(pop, thisOutpath);

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
Rcpp::NumericVector get_values_1d(const int nOctaves, const double frequency,
                                  const double increment, const int nValues) {
    // get seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    // make perlin noise
    module::Perlin noise;
    noise.SetOctaveCount(nOctaves);
    noise.SetFrequency(frequency);
    noise.SetPersistence(0.5);
    noise.SetSeed(static_cast<int> (seed));

    // make a vector to hold values
    Rcpp::NumericVector sampleVals (nValues);

    double coordX = static_cast<double> (seed);
    double coordY, coordZ;
    coordY = coordZ = 0.0;
    for (size_t i = 0; i < nValues; ++i) {
        sampleVals[i] = noise.GetValue(coordX, coordY, coordZ);
        coordX += increment;
    }
    return sampleVals;
}

// test seed setting
//' Test seed and random number generation.
//'
//' @param increment The increment in the X coordinate.
//' @param nValues How many steps, each of \code{increment} magnitude, to take.
// [[Rcpp::export]]
Rcpp::NumericVector get_rand_values(const int nValues, const double increment) {
    // get seed
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

    Rcpp::NumericVector sampleVals(nValues);

    // for (size_t i = 0; i < nValues; ++i) {
    //     sampleVals[i] = coordX;//noise.GetValue(coordX, coordY, coordZ);
    //     coordX += increment;
    // }

    return sampleVals;

}

#endif // SIMULATION_H
