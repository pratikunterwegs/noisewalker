
#include <iostream>
#include <chrono>
#include "parameters.hpp"
#include "agent.hpp"
#include "datatypes.hpp"
#include "pathospread.hpp"
#include "predation.hpp"
#include "simulation.hpp"
#include "FastNoiseLite.h"
#include <Rcpp.h>

using namespace Rcpp;

/// function to evolve population
Rcpp::List evolvePop(std::vector<Agent> &pop,
               const int genmax,
               const int timesteps,
               FastNoiseLite &noise,
               const float landsize,
               const float clamp,
               const float perception,
               const int directions,
               const float costMove,
               const int scenario,
               const float pTransmit,
               const float costInfection,
               const float costPredAvoid,
               const bool recordPos)
{
    genData thisGenData;
    posData thisPosData;
    float scale_time = 0.001f; // no more landscape change

    // assign generation after which pathogen is introduced
    int genPartition = genmax; // never introduced
    if (scenario == 1) {
        // introduced from generation 0
        genPartition = 0;
    } else if (scenario == 2) {
        // pathogen introduced 2/3rds through the sim
        genPartition = static_cast<int> ((2.f / 3.f) * genmax);
    }

    // loop through generations
    for (int gen = 0; gen < genmax; ++gen) {
        if (gen >= genPartition) {
            popIntroPathogen(pop, 1);
        }

        for (int t = 0; t < timesteps; ++t) {

            // make rtree
            bgi::rtree< value, bgi::quadratic<16> > agentRtree = makeRtree(pop);
            // scale t by minor value
            popMoveForageCompete(pop, noise, agentRtree, static_cast<float>(t) * scale_time,
                perception, directions, landsize,
                clamp, costMove); // set manually

            // infection dynamics
            // if (gen >= genPartition) {
            //     // popPathogenSpread(pop, perception, pTransmit, t, agentRtree);
            //     // popPathogenCost(pop, costInfection, t);
            // }

            // predation dynamics
            // popPredationRisk(pop, costPredAvoid, perception, agentRtree);
        }

        // subtract cost of traits?
        // for (size_t i = 0; i < pop.size(); i++)
        // {
        //     pop[i].energy -= ((std::fabs(pop[i].coefFood) + std::fabs(pop[i].coefNbrs)));
        // }
        
        thisGenData.updateGenData(pop, gen);

        if (recordPos) thisPosData.updatePosData(pop, gen);
        // reproduce once generation is done
        doReproduce(pop, landsize);
    }

    if (recordPos)
    {
        return Rcpp::List::create(
            Named("gendata") = thisGenData.returnGenData(),
            Named("posdata") = thisPosData.returnPosData()
        );
    } else {
        return Rcpp::List::create(
            Named("gendata") = thisGenData.returnGenData()
        );
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
//' @param perception The perception range.
//' @param directions The number of points at which agents sense resources,
//' at a fixed distance of \code{perception} units away from them.
//' @param costMove The cost per move; distance moved is assumed constant.
//' @param freqRes Frequency of noise. May be thought of as large scale
//' variability. May be any double value between 1.0 and 16.0. Higher values
//' mean more patchy landscapes.
//' @param landsize The size of the landscape after which movement is wrapped.
//' Actually, the landscape is infinite, and does not have a repeating pattern.
//' @param clamp The threshold value of the landscape below which, the agents
//' sense and receive zero resources. Needed because noise has values -1 to +1.
//' @param random_traits Should traits be initialised -1 to +1 or at 0.
//' @param scenario The pathogen co-evolution scenario. Defaults to 0, "none",
//' and no pathogen is introduced. If 1, "ancestral", the pathogen is introduced
//' from generation zero, the start of the simulation. If 2, "spillover", the
//' pathogen is introduced after 2/3 of the simulation.
//' @param pTransmit The probability a disease transmits.
//' @param costInfection The per-timestep cost of being infected.
//' @param costPredAvoid The cost of predation avoidance.
//' @param recordPos Whether to record the final position of individuals.
//' @return A dataframe of evolved pop strategy count.
// [[Rcpp::export]]
S4 run_noisewalker(
        const int popsize,
        const int genmax, 
        const int timesteps,
        const float perception,
        const int directions,
        const float costMove, 
        const double freqRes,
        const float landsize,
        const float clamp,
        const bool random_traits,
        const int scenario,
        const float pTransmit,
        const float costInfection,
        const float costPredAvoid,
        const float recordPos) {
    
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
    noise.SetSeed(seed);
    noise.SetFrequency(freqRes);
    
    // do evolution
    Rcpp::List thisData = evolvePop(pop, genmax, timesteps, noise, landsize, 
                                    clamp, perception,
                                    directions, costMove, scenario,
                                    pTransmit, costInfection, costPredAvoid, recordPos);
    // generation data
    Rcpp::List gendata = thisData["gendata"];

    // parameter list
    Rcpp::List param_list = Rcpp::List::create(
        Named("generations") = genmax,
        Named("pop_size") = popsize,
        Named("pop_density") = static_cast<float>(popsize) / landsize,
        Named("patchiness") = freqRes
        // Named("dispersal") = dispersal,
    );

    // create S4 class pathomove output and fill slots
    S4 x("sim_output");
    x.slot("parameters") = Rcpp::wrap(param_list);
    x.slot("generations") = Rcpp::wrap(thisData["gens"]);
    x.slot("trait_data") = Rcpp::wrap(gendata["pop_data"]);

    return(x);
}

