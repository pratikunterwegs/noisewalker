#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <libnoise/noise.h>
#include "noiseutils.h"
#include <parameters.hpp>
#include <agent.hpp>

/// function to evolve population
void evolvePop(std::vector<Agent> &pop,
               const int genmax, const int timesteps,
               const double tempRate,
               module::Perlin landscape)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    double coordZ = 0.0;
    for (int gen = 0; gen < genmax; ++gen) {
        for (int t = 0; t < timesteps; ++t) {
            // if gen has not changed then move and forage
            popMoveForage(pop, coordZ, landscape);
            coordZ += tempRate;
        }
        doReproduce(pop);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "pop evolve time: " << duration << "\n";

}

/* simulation wrapper */
/// do simulations
void do_simulation(std::vector<std::string> cliArgs){
    // gather cli args
    const int genmax = std::stoi(cliArgs[1]);
    const int timesteps = std::stoi(cliArgs[2]);
    const int nOctaves = std::stoi(cliArgs[3]);
    const double frequency = std::stod(cliArgs[4]);
    const double tempRate = std::stod(cliArgs[5]);
    const int newSrange = std::stof(cliArgs[6]);
    std::string rep = cliArgs[7];

    // init pop & landscape
    std::vector<Agent> pop (popSize);
    module::Perlin noise;
    noise.SetOctaveCount(nOctaves);
    noise.SetFrequency(frequency);
    noise.SetPersistence(0.5);

    forceSrange(pop, newSrange);

    // print outpath for test
    // std::cout << "data at " << thisOutpath[0] << thisOutpath[1] << "\n";

    // do evolution
    evolvePop(pop, genmax, timesteps, tempRate, noise);
    
    std::cout << "pop evolved!" << "\n";

}

#endif // SIMULATION_H
