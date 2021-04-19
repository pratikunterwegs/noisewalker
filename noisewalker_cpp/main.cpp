#include <QCoreApplication>
#include "../src/parameters.h"
#include "../src/FastNoiseLite.h"
#include "../src/agent.h"
#include "../src/noisewalker_tools.h"
#include "../src/simulation.cpp"

int main()
{
    // set the random number generation etc
    const gsl_rng_type * T;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);

    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());

    // init pop
    std::vector<Agent> pop (50, Agent(1, 1.f));
    // random position
    popRandomPos(pop, landsize);
    // random weights
    popRandomWeights(pop);

    // make the ancestral landscape
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetFrequency(2.0);
    noise.SetFractalOctaves(2);

    // do evolution
    Rcpp::List thisData = evolvePop(pop, 50, 10, 0.01, noise, landsize);

    std::cout << "pop evolved";
    return 0;
}
