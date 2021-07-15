#include "../src/parameters.hpp"
#include "../src/FastNoiseLite.h"
#include "../src/agent.hpp"
#include "../src/datatypes.hpp"
#include "../src/simulation.hpp"
#include <chrono>

int main()
{
    // set the random number generation etc
    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    rng.seed(seed);

    // init pop
    std::vector<Agent> pop (10);
    // random position
    float landsize = 10.f;
    popRandomPos(pop, landsize);
    // random weights
    popRandomTraits(pop);

    // make the ancestral landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(2.0);

    // do evolution
    Rcpp::List thisData = evolvePop(pop, 10, 10, noise, 10.f, 0.f, 0.05f, 4, 0.1f, true);

    std::cout << "pop evolved";
    return 0;
}
