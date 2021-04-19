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
Layer< Neuron<4, activation::rtlu>, 3>, // for now, 4 input for energy cues
Layer< Neuron<3, activation::rtlu>, 3>,
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
    Agent(const int strategy_int, const float perception_range) :
        // count agents correctly heritable parameter is s_range
        energy(0.01f),
        perception(perception_range),
        y(0.f),
        x(0.f),
        annMove(0.f),
        strategy(strategy_int) // 1 = scout, 2 = forecaster, 3 = lorekeeper

    {}
    ~Agent() {}

    // agent attributes
    float energy;
    float perception;
    float x, y;
    Ann annMove;
    int strategy;

    // agent functions
    void doMove(FastNoiseLite landscape, const double landsize);
    void doForage(FastNoiseLite landscape);
    std::vector<float> getAnnWeights();
    
    void randomWeights();
    void randomPosition(const float landsize);
    
    
    std::array<float, 2> annOutput (const float v1, const float v2, 
                                    const float v3, const float v4);
};

/// function to count strategy proportions
std::vector<double> getPopStrategyProp (std::vector<Agent> &pop) {
  std::vector<int> vCount (3, 0);
  for(size_t i = 0; i < pop.size(); i++) {
    vCount[pop[i].strategy - 1]++;
  }
  std::vector<double> vProp (3, 0.f);
  for(size_t i = 0; i < vCount.size(); i++) {
    vProp[i] = static_cast<double> (vCount[i]) / static_cast<double> (pop.size());
  }
  return vProp;
}

// strategy probabilities
std::vector<double> strategyProb (3, 0.33);
std::discrete_distribution <> rndStrategy (strategyProb.begin(), strategyProb.end());

/// initialise the population at random positions
void Agent::randomPosition(const float landsize) {
    x = gsl_rng_uniform(r) * static_cast<double>(landsize);
    y = gsl_rng_uniform(r) * static_cast<double>(landsize);
}

/// individual random weights
void Agent::randomWeights() {
    for (auto& w : annMove) {
        // probabilistic mutation of ANN using GSL
        // using GSL for historical reasons
        w = static_cast<float> (gsl_ran_gaussian(r, 2.0));
    }
}

/// initialise with random position
void popRandomPos(std::vector<Agent> &pop, const float landsize) {
  for(auto& indiv : pop) {
    indiv.randomPosition(landsize);
  }
}

/// initialise with random weights
void popRandomWeights(std::vector<Agent> &pop) {
    for(auto& indiv : pop) {
        indiv.randomWeights();
    }
}

/// get ANN output
std::array<float, 2> Agent::annOutput(const float v1, const float v2,
                                      const float v3, const float v4) {
    // def inputs
    Ann::input_t inputs;
    // pass inputs
    inputs[0] = v1;
    inputs[1] = v2;
    inputs[2] = v3;
    inputs[3] = v4;
    // get output
    auto distAngle = annMove(inputs);

    return distAngle;
}

/// agent function to choose a new position
void Agent::doMove(FastNoiseLite noise, const double landsize) {
    // agents use ANN to move
    // ANN senses values at some offset based on strategy
    // output 0 is distance, 1 is angle
    std::array<float, 2> output;
    if (strategy == 1) { // sense around
      output = annOutput((noise.GetNoise(x + perception, y + perception)), // x+1,y+1
                         (noise.GetNoise(x - perception, y + perception)), // x-1,y+1
                         (noise.GetNoise(x + perception, y - perception)), // x+1,y-1
                         (noise.GetNoise(x - perception, y - perception))  // x-1,y-1
      ); 
    } else if (strategy == 2) { // sense here but four timesteps ahead
      output = annOutput((noise.GetNoise(x, y, perception)), 
                         (noise.GetNoise(x, y, perception * 2)), 
                         (noise.GetNoise(x, y, perception * 3)), 
                         (noise.GetNoise(x, y, perception * 4))
      );
    } else {
      output = annOutput((noise.GetNoise(x, y, -perception)), 
                         (noise.GetNoise(x, y, -perception * 2)), 
                         (noise.GetNoise(x, y, -perception * 3)), 
                         (noise.GetNoise(x, y, -perception * 4))
      );
    }

    // take either output or 10% body mass whichever is lower
    // only consider abs value to determine the magnitude
    float move_dist = static_cast<float> (abs(output[0]));
    float angle = static_cast<float> (output[1]); // we assume this is degrees
    angle = angle * M_PI / 180.0;
    
    // get new position
    x = x + (move_dist * cos(angle));
    y = y + (move_dist * sin(angle));
    
    // make the move on the wrapped landscape
    x = fmod(x, landsize);
    y = fmod(y, landsize);
}

/// agent function to forage
void Agent::doForage(FastNoiseLite landscape) {
    float energy_here = (landscape.GetNoise(x, y));
    energy += energy_here < 0.f ? 0.f : energy_here;
}

/* population level functions */
/// population moves about and forages
void popMoveForage(std::vector<Agent>& pop, FastNoiseLite landscape,
                   const double landsize) {
    for(auto& indiv : pop) {
        indiv.doMove(landscape, landsize);
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
        vecFitness.push_back(static_cast<double> (pop[a].energy));
    }
    normaliseFitness(vecFitness);

    // weighted lottery
    std::discrete_distribution<> weightedLottery(vecFitness.begin(), vecFitness.end());

    // create new population
    std::vector<Agent> tmpPop(popSize, Agent(1, 1.f));

    // assign parents
    for (size_t a = 0; static_cast<int>(a) < popSize; a++) {

        size_t idParent = static_cast<size_t> (weightedLottery(rng));

        // replicate ANN
        tmpPop[a].annMove = pop[idParent].annMove;
        // get parent position
        tmpPop[a].x = pop[idParent].x;
        tmpPop[a].y = pop[idParent].y;
        tmpPop[a].strategy = pop[idParent].strategy;

        // mutate ann and strategy
        for (auto& w : tmpPop[a].annMove) {
            // probabilistic mutation of ANN using GSL
            // using GSL for historical reasons
            if (gsl_ran_bernoulli(r, static_cast<double>(mProb)) == 1) {
                w += static_cast<float> (gsl_ran_cauchy(r, static_cast<double>(mShift)));
            }
        }
        if (gsl_ran_bernoulli(r, static_cast<double>(mProb)) == 1) {
          // draw strategy from discrete distribution
          tmpPop[a].strategy = rndStrategy(rng);
        }
    }
    // swap tmp pop for pop
    std::swap(pop, tmpPop);
    // randomise position
    popRandomPos(tmpPop, landsize);
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
