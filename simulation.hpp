#ifndef SIMULATION_H
#define SIMULATION_H

#include <iostream>
#include <parameters.hpp>
#include <landscape.hpp>
#include <agent.hpp>

/// function to evolve population
/// THIS WHOLE THING NEEDS TO CHANGE
void evolvePop(std::vector<Agent> &pop,
               const int genmax, const int timesteps,
               module::Perlin landscape)
{
    auto t1 = std::chrono::high_resolution_clock::now();
    // loop through timesteps
        for (int t = 0; t < maxTimesteps; ++t) {
            if (t % timePerGen == 0) {
                // reproduce
                doReproduce(pop);
            }
            // if gen has not changed then move and forage
            popMoveForage(pop, t, landscape);
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
    const int newSrange = std::stof(cliArgs[5]);
    std::string rep = cliArgs[6];

    // init pop & landscape
    std::vector<Agent> pop (popSize);
    module::Perlin landscape = makeNoise(nOctaves, frequency);
    // force sensory range
    forceSrange(pop, newSrange);

    // print outpath for test
    // std::cout << "data at " << thisOutpath[0] << thisOutpath[1] << "\n";

    // do evolution
    evolvePop(pop, genmax, timesteps, landscape);
    
    std::cout << "pop evolved!" << "\n";

}

#endif // SIMULATION_H
