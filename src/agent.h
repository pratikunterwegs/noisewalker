#ifndef AGENT_H
#define AGENT_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include "parameters.h"
#include "FastNoiseLite.h"
#include "ann-lib-linux/rnd.hpp"
#include "ann-lib-linux/rndutils.hpp"
#include "ann-lib-linux/ann2.hpp"

using namespace ann;

// spec ann structure
using Ann = Network<float,
Layer< Neuron<2, activation::rtlu>, 3>, // for now, 4 input for energy cues
//    Layer< Neuron<3, activation::rtlu>, 3>,
Layer< Neuron<3, activation::rtlu>, 1> // two outputs, distance and angle
>;

// clear node state
struct flush_rec_nodes {
    template <typename Neuron, typename T>
    void operator()(T* state, size_t /*layer*/, size_t /*neuron*/)
    {
        auto it = state + Neuron::feedback_scratch_begin;
        for (auto i = 0; i < Neuron::feedback_scratch; ++i) {
            it[i] = T(0);
        }
    }
};

// Agent class
class Agent {
public:
    Agent() :
        // count agents correctly heritable parameter is s_range
        energy(1.f),
        sRange(0.1f),
        x(0.f),
        mass(1.0),
        annMove(0.f)

    {}
    ~Agent() {}

    // Agents need a brain, an age, fitness, and movement decision
    float energy;
    float sRange;
    float x; 
    float mass;
    std::array<float, 1> annOutput (const float v1, const float v2);
    Ann annMove;

    // do move
    // void wrapPosition(float newX, float newY, const float maxPos, const float minPos);
    void doMove(FastNoiseLite landscape);
    void doForage(FastNoiseLite landscape);
    std::vector<float> getAnnWeights();
    void randomWeights();
};


/// functions to initialise the population
/// force the sensory range of the population
void forceSrange(std::vector<Agent>& pop, const double s){
  for(size_t i = 0; i < pop.size(); i++){
    pop[i].sRange = s;
  }
}

/// initialise the population at random positions
void randomPosition(std::vector<Agent> &pop) {
    for(size_t i = 0; i < pop.size(); i++){
      pop[i].x = gsl_rng_uniform(r) * static_cast<double>(landsize);
    //   pop[i].y = gsl_rng_uniform(r) * static_cast<double>(landsize);
    }
}

/// individual random weights
void Agent::randomWeights() {
    for (auto& w : annMove) {
        // probabilistic mutation of ANN using GSL
        // using GSL for historical reasons
        w = static_cast<float> (gsl_ran_gaussian(r, 2.0));
    }
}

/// initialise with random weights
void popRandomWeights(std::vector<Agent> &pop) {
    for(auto& indiv : pop) {
        indiv.randomWeights();
    }
}

/// get ANN output
std::array<float, 1> Agent::annOutput(const float v1, const float v2) {
    // def inputs
    Ann::input_t inputs;
    // pass inputs
    inputs[0] = v1 > clamp ? v1 : 0.f;
    inputs[1] = v2 > clamp ? v2 : 0.f;
    // get output
    auto distAngle = annMove(inputs);

    return distAngle;
}

/// agent function to choose a new position
void Agent::doMove(FastNoiseLite noise) {
    // agents use ANN to move
    // ANN senses Clamp values at some offset
    // output 0 is distance, 1 is angle
    std::array<float, 1> output = annOutput((noise.GetNoise(x + sRange, 0.f)),
                                            (noise.GetNoise(x - sRange, 0.f)));

    // take either output or 10% body mass whichever is lower
    // only consider abs value to determine the magnitude
    float move_dist = static_cast<double> (abs(output[0]));

    if (move_dist > mass * mass_move_ratio) {
        move_dist = mass * mass_move_ratio;
    }                                      
    // now assign the sign
    move_dist = move_dist * (output[0] >= 0.f ? 1.f : -1.f);

    // new unwrapped position, returns floats?
    x += move_dist;

    // nov3 -- no distance penalty
    // penalise the distance actually moved, not the distance chosen
    // agent max penalty is move cost * mass * mass_move_ratio
    // energy -= move_cost * abs(move_dist);
}

/// agent function to forage
void Agent::doForage(FastNoiseLite landscape) {

    float energy_here = (landscape.GetNoise(x, 0.f));
    energy += energy_here < clamp ? 0.f : energy_here;

    // lose energy due to mass upkeep
    energy -= mass * mass_cost;

}

/* population level functions */
/// population moves about and forages
void popMoveForage(std::vector<Agent>& pop, FastNoiseLite landscape) {
    for(auto& indiv : pop) {
        indiv.doMove(landscape);
        indiv.doForage(landscape);
    }
}

/// minor function to normalise vector
void normaliseFitness(std::vector<double> &vecFitness) {
    float minFitness = 0.0f;
    float maxFitness = 1.0f;
    // get min and max fitness
    for (size_t vecVal = 0; vecVal < vecFitness.size(); ++vecVal) {
        if (vecFitness[vecVal] < minFitness) {
            minFitness = vecFitness[vecVal];
        }
        if (vecFitness[vecVal] > maxFitness) {
            maxFitness = vecFitness[vecVal];
        }
    }
    // rescale values
    for (size_t vecVal = 0; vecVal < vecFitness.size(); ++vecVal) {
        vecFitness[vecVal] -= minFitness;
        vecFitness[vecVal] = vecFitness[vecVal] / (maxFitness - minFitness);
        // add a small value to avoid zero values
        vecFitness[vecVal] += 0.0000000001;
        assert(vecFitness[vecVal] > 0.0 && "Agent energy is 0!");
    }

}

/// pop reproduces
void doReproduce(std::vector<Agent>& pop) {
    // make fitness vec
    std::vector<double> vecFitness;
    for (size_t a = 0; static_cast<int>(a) < popSize; a++)
    {
        vecFitness.push_back(static_cast<double> (pop[a].energy - 
            // add a cost to high mass
            (pop[a].mass * mass_cost * timePerGen)));
    }
    normaliseFitness(vecFitness);

    // weighted lottery
    std::discrete_distribution<> weightedLottery(vecFitness.begin(), vecFitness.end());

    // create new population
    std::vector<Agent> tmpPop(popSize);

    // assign parents
    for (size_t a = 0; static_cast<int>(a) < popSize; a++) {

        size_t idParent = static_cast<size_t> (weightedLottery(rng));

        // replicate ANN
        tmpPop[a].annMove = pop[idParent].annMove;
        // get parent position
        tmpPop[a].x = pop[idParent].x;
        tmpPop[a].mass = pop[idParent].mass;

        // mutate ann
        for (auto& w : tmpPop[a].annMove) {
            // probabilistic mutation of ANN using GSL
            // using GSL for historical reasons
            if (gsl_ran_bernoulli(r, static_cast<double>(mProb)) == 1) {
                w += static_cast<float> (gsl_ran_cauchy(r, static_cast<double>(mShift)));
            }
        }

        // mutate mass
        if (gsl_ran_bernoulli(r, static_cast<double>(mProb)) == 1) {
                tmpPop[a].mass += static_cast<float> (gsl_ran_cauchy(r, static_cast<double>(mShift)));
                if (tmpPop[a].mass < mass_min) {
                    tmpPop[a].mass = mass_min;
                }
            }
    }

    // swap tmp pop for pop
    std::swap(pop, tmpPop);
    // randomise position
    randomPosition(pop);
    tmpPop.clear();
}

/* tools for agent weights */
/// get individual weights
std::vector<float> Agent::getAnnWeights() {
    // make vec
    std::vector<float> vecWeights;
    for (auto& w : annMove) {
        vecWeights.push_back(w);
    }
    return vecWeights;
}

/// get population weights
std::vector<std::vector<float> > getPopAnnWts(std::vector<Agent> &pop) {
    // vec of indivs
    std::vector<std::vector<float> > popWeights(popSize);
    for(size_t indiv = 0; static_cast<int>(indiv) < popSize; ++indiv) {
        popWeights[indiv] = pop[indiv].getAnnWeights();
    }

    return popWeights;
}

#endif // AGENT_H
