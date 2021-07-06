
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
               const float costMove,
               const bool allow_compete,
               const bool allow_coop)
{
    genData thisGenData;
    float scale_time = 0.1f;
    // loop through generations
    for (int gen = 0; gen < genmax; ++gen) {
        Rcpp::Rcout << "generation: " << gen << "\r";
        for (int t = 0; t < timesteps; ++t) {
            // scale t by minor value
            popMoveForageCompete(pop, noise, risk, static_cast<float>(t) * scale_time, 
                perception, directions, landsize,
                clamp, costMove, allow_compete, allow_coop); // set manually
        }

        // subtract cost of traits?
        // for (size_t i = 0; i < pop.size(); i++)
        // {
        //     pop[i].energy -= ((std::fabs(pop[i].coefFood) + std::fabs(pop[i].coefNbrs)));
        // }
        
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
//' @param random_traits Should traits be initialised -1 to +1 or at 0.
//' @param allow_compete Should agents compete. Controls downstream functions.
//' @param allow_coop Should agents share costs on the risk landscape.
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
        const float clamp,
        const bool random_traits,
        const bool allow_compete,
        const bool allow_coop) {

    if(allow_coop) {
        assert(allow_compete && "allow coop TRUE requires allow compete TRUE");
    }
    
    // set up seed etc
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);
    
    // init pop
    std::vector<Agent> pop (popsize);
    // random position
    popRandomPos(pop, landsize); // landsize is fixed in parameters.hpp
    // random weights
    if (random_traits) {
        popRandomTraits(pop);
    }
    
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
                                    directions, costMove, allow_compete, 
                                    allow_coop);

    return thisData;
}

