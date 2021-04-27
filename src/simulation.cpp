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
Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax, const int timesteps,
               const float t_increment,
               FastNoiseLite noise)
{
    fitnessData thisFitnessData;
    genData thisGenData;
    float time = 0.f;
    for (int gen = 0; gen < genmax; ++gen) {
        for (int t = 0; t < timesteps; ++t) {
            time += t_increment;
            // if gen has not changed then move and forage
            popMoveForage(pop, noise, time);
        }
        time = 0.f;
        thisGenData.updateGenData(pop, gen);
        thisFitnessData.updateFitnessData(pop, gen);
        doReproduce(pop);
    }
    return Rcpp::List::create(
        Named("pop_comp") = thisGenData.returnGenData(),
        Named("fitness") = thisFitnessData.returnFitnessData()
    );
}


//' Runs the noisewalker simulation.
//'
//' @description Run the noisewalker simulation using parameters passed as
//' arguments to the corresponding R function.
//' 
//' @param popsize The population size.
//' @param genmax The maximum number of generations per simulation.
//' @param timesteps The number of timesteps per generation.
//' @param t_increment Rate of landscape change.
//' @param nOctaves Number of octaves. May be thought of as small scale 
//' variability. Must be an integer value between 1 and 8. Higher values
//' result in landscapes with more small scale noise.
//' @param frequency Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param landsize The landscape size.
//' @return A dataframe of evolved pop strategy count.
// [[Rcpp::export]]
Rcpp::List run_noisewalker(
        const int popsize,
        const int genmax, const int timesteps,
        const float t_increment,
        const int nOctaves, const double frequency,
        const double landsize) {
    
    // set the random number generation etc
    // const gsl_rng_type * T;
    // gsl_rng_env_setup();
    // T = gsl_rng_default;
    // r = gsl_rng_alloc (T);
    
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng (seed);
    
    // init pop
    std::vector<Agent> pop (popsize, Agent(1, 1.f));
    // random position
    popRandomPos(pop, landsize);
    // random weights
//    popRandomWeights(pop);
    popRandomStrategy(pop);
    
    // make the ancestral landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(nOctaves);
    
    // do evolution
    Rcpp::List thisData = evolvePop(pop, genmax, timesteps, t_increment, noise);

    return thisData;
}

#endif // SIMULATION_H
