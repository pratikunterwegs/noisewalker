#ifndef AGENT_H
#define AGENT_H

/// code to make agents
#include <vector>
#include <cassert>
#include "parameters.hpp"
#include "landscape.hpp"
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
    void doMove(const float now);
    void doForage(const float now);
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
void Agent::doMoveForage(const float now) {
    // agents use ANN to move
    // ANN senses perlin values at some offset
    Ann::input_t inputs;

    inputs[0] = landscape.getValues(x + 1, y + 1, now);
    inputs[1] = landscape.getValues(x + 1, y - 1, now);
    inputs[2] = landscape.getValues(x - 1, y + 1, now);
    inputs[3] = landscape.getValues(x - 1, y - 1, now);

    // output 0 is distance, 1 is angle
    auto output = annMove(inputs);

    // new unwrapped position
    float newX = x + (output[0] * cosf(output[1])); // is the angle correctly handled?
    float newY = y + (output[0] * sinf(output[1]))

    // wrap new position
    wrapPosition(newX, newY, 0.f, landsize); // needs a landsize in params

}

/// agent function to forage
void Agent::doForage(const float now) {

    energy += (landscape.getValues(x + 1, y + 1, now) +
               landscape.getValues(x + 1, y - 1, now) +
               landscape.getValues(x - 1, y + 1, now) +
               landscape.getValues(x - 1, y - 1, now)) / 4.f;
}

#endif // AGENT_H
