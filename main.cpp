#include <iostream>
#include <libnoise/noise.h>
#include "parameters.hpp"
#include "agent.hpp"
#include "simulation.hpp"

int main(int argc, char *argv[])
{
    std::vector<std::string> cliArgs(argv, argv+argc);

    // init gsl rng
    const gsl_rng_type * T;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);

    unsigned seed = static_cast<unsigned> (std::chrono::system_clock::now().time_since_epoch().count());
    gsl_rng_set(r, seed);

    do_simulation(cliArgs);

    return 0;
}
