#ifndef AGENT_H
#define AGENT_H

/// code to make agents
#include <vector>
#include <cassert>
#include "parameters.hpp"
#include <libnoise/noise.h>
#include "noiseutils.h"
#include "ann-lib-linux/rnd.hpp"
#include "ann-lib-linux/rndutils.hpp"
#include "ann-lib-linux/ann2.hpp"

using namespace ann;

// spec ann structure
using Ann = Network<float,
Layer< Neuron<4, activation::rtlu>, 3>, // for now, 4 input for energy cues
//    Layer< Neuron<3, activation::rtlu>, 3>,
Layer< Neuron<3, activation::rtlu>, 2> // two outputs, distance and angle
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
        sRange(1.f),
        x(0.f), y(0.f),
        annMove(0.f)

    {}
    ~Agent() {}

    // Agents need a brain, an age, fitness, and movement decision
    float energy, sRange, x, y;
    Ann annMove;

    // do move
    void wrapPosition(float newX, float newY, const float maxPos, const float minPos);
    void doMove(module::Perlin landscape, const float now);
    void doForage(module::Perlin landscape, const float now);
    std::vector<float> getAnnWeights();
};


/// functions to initialise the population
/// force the sensory range of the population
void forceSrange(std::vector<Agent>& pop, const float s){
  for(size_t i = 0; i < pop.size(); i++){
    pop[i].sRange = s;
  }
}

/// initialise the population at random positions
/// WIP

/// generic wrapper function for floats
// assumes position minimum is zero
float wrapper(float x, float min, float max) {
        return min + fmod((max - min) + fmod(x - min, max - min), max - min);
}

// distance wrapper float
/// agent function to wrap position
void Agent::wrapPosition(float newX, float newY,
                   const float minPos, const float maxPos) {

    x = wrapper(newX, minPos, maxPos);
    y = wrapper(newY, minPos, maxPos);
}

/// agent function to choose a new position
void Agent::doMove(module::Perlin landscape, const float now) {
    // agents use ANN to move
    // ANN senses perlin values at some offset
    Ann::input_t inputs;

    inputs[0] = static_cast<float>(landscape.GetValue(x + 1, y + 1, now));
    inputs[1] = static_cast<float>(landscape.GetValue(x + 1, y - 1, now));
    inputs[2] = static_cast<float>(landscape.GetValue(x - 1, y + 1, now));
    inputs[3] = static_cast<float>(landscape.GetValue(x - 1, y - 1, now));

    // output 0 is distance, 1 is angle
    auto output = annMove(inputs);

    // new unwrapped position
    float newX = x + (output[0] * cosf(output[1])); // is the angle correctly handled?
    float newY = y + (output[0] * sinf(output[1]));

    // wrap new position
    wrapPosition(newX, newY, 0.f, landsize); // needs a landsize in params

}

/// agent function to forage
void Agent::doForage(module::Perlin landscape, const float now) {

    energy += static_cast<float> (landscape.GetValue(x + 1, y + 1, now) +
               landscape.GetValue(x + 1, y - 1, now) +
               landscape.GetValue(x - 1, y + 1, now) +
               landscape.GetValue(x - 1, y - 1, now)) / 4.f;
}

/* population level functions */
/// population moves about and forages
void popMoveForage(std::vector<Agent>& pop, const float now, module::Perlin landscape) {
    for(auto& indiv : pop) {
        indiv.doMove(landscape, now);
        indiv.doForage(landscape, now);
    }
}

/// minor function to normalise vector
void normaliseFitness(std::vector<double> &vecFitness) {
    float minFitness = 0.0;
    float maxFitness = 1.0;
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
        vecFitness.push_back(static_cast<double> (pop[a].energy));
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
        tmpPop[a].y = pop[idParent].y;

        // mutate ann
        for (auto& w : tmpPop[a].annMove) {
            // probabilistic mutation of ANN using GSL
            // using GSL for historical reasons
            if (gsl_ran_bernoulli(r, static_cast<double>(mProb)) == 1) {
                w += static_cast<float> (gsl_ran_cauchy(r, static_cast<double>(mShift)));
            }
        }
    }

    // swap tmp pop for pop
    std::swap(pop, tmpPop);
    tmpPop.clear();
}

#endif // AGENT_H
