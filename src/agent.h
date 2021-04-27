#ifndef AGENT_H
#define AGENT_H

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>
#include "parameters.h"
#include "FastNoiseLite.h"
#include "ann-lib-windows/rnd.hpp"
#include "ann-lib-windows/rndutils.hpp"
#include "ann-lib-windows/ann2.hpp"

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
        x(0.f),
        y(0.f),
        annMove(0.f),
        strategy(strategy_int), // 0 = scout, 1 = forecaster, 2 = lorekeeper, 3 = random, 4 = one forward, three around
        moved(0.0)

    {}
    ~Agent() {}

    // agent attributes
    float energy;
    float perception;
    float x, y;
    Ann annMove;
    int strategy;
    double moved;

    // agent functions
    void doMove(FastNoiseLite landscape, const float t_);
    void doForage(FastNoiseLite landscape, const float t_);
    std::vector<float> getAnnWeights();
    
    void randomWeights();
    void randomPosition();
    void randomStrategy();
    
    
    std::array<float, 2> annOutput (const float v1, const float v2, 
                                    const float v3, const float v4);
};

// distributions and strategy probabilities
std::vector<double> strategyProb (5, 0.2);
std::discrete_distribution <> rndStrategy (strategyProb.begin(), strategyProb.end());
std::uniform_real_distribution <> rndPos (0.0, landsize);
std::normal_distribution <> rndWt (0.0, 2.0);
std::normal_distribution <> rndErr (0.0, 0.1);
std::bernoulli_distribution <> rndBnl (mProb);
std::cauchy_distribution <> rndMut ()

/// initialise the population at random positions
void Agent::randomPosition() {
    x = rndPos(rng);
    y = rndPos(rng);
}

/// individual random weights
void Agent::randomWeights() {
    for (auto& w : annMove) {
        // probabilistic mutation of ANN using GSL
        // using GSL for historical reasons
        w = static_cast<float> (rndWt(rng));
    }
}

void Agent::randomStrategy(){
    strategy = rndStrategy(rng);
}

/// initialise with random position
void popRandomPos(std::vector<Agent> &pop, const float landsize) {
  for(auto& indiv : pop) {
    indiv.randomPosition();
  }
}

/// initialise with random weights
void popRandomWeights(std::vector<Agent> &pop) {
    for(auto& indiv : pop) {
        indiv.randomWeights();
    }
}

void popRandomStrategy(std::vector<Agent> &pop){
    for(auto& indiv : pop) {
        indiv.randomStrategy();
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
void Agent::doMove(FastNoiseLite noise, const float t_) {
    // agents use ANN to move
    // ANN senses values at some offset based on strategy
    // output 0 is distance, 1 is angle
    std::array<float, 2> output;
    switch (strategy) {
    case 0:
        output = annOutput((noise.GetNoise(x + perception, y + perception, t_)), // x+1,y+1
                           (noise.GetNoise(x - perception, y + perception, t_)), // x-1,y+1
                           (noise.GetNoise(x + perception, y - perception, t_)), // x+1,y-1
                           (noise.GetNoise(x - perception, y - perception, t_))  // x-1,y-1
        );
        break;
    case 1:
        output = annOutput((noise.GetNoise(x, y, t_ + perception)),
                           (noise.GetNoise(x, y, t_ + perception * 1.5f)) + static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x, y, t_ + perception * 1.75f)) + static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x, y, t_ + perception * 2.f)) + static_cast<float>(rndErr(rng))
        );
        break;
    case 2:
        output = annOutput((noise.GetNoise(x, y, t_ -perception)),
                           (noise.GetNoise(x, y, t_ -perception * 1.5f)) +  static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x, y, t_ -perception * 1.75f)) +  static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x, y, t_ -perception * 2.f)) +  static_cast<float>(rndErr(rng))
        );
        break;
    case 3:
        output = annOutput(static_cast<float>(rndWt(rng)),
                           static_cast<float>(rndWt(rng)),
                           static_cast<float>(rndWt(rng)),
                           static_cast<float>(rndWt(rng))
        );
        break;
    case 4:
        output = annOutput((noise.GetNoise(x, y, t_ + perception)),
                           (noise.GetNoise(x - perception, y + perception, t_)) + static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x + perception, y - perception, t_)) + static_cast<float>(rndErr(rng)),
                           (noise.GetNoise(x - perception, y - perception, t_)) + static_cast<float>(rndErr(rng))
        );
        break;
    }

    // take either output or 10% body mass whichever is lower
    // only consider abs value to determine the magnitude
    float move_dist = static_cast<float> (abs(output[0]));
    float angle = static_cast<float> (output[1]); // we assume this is degrees
    angle = angle * M_PI / 180.0;
    
    moved += move_dist;

    // get new position
    x = x + (move_dist * cos(angle));
    y = y + (move_dist * sin(angle));
}

/// agent function to forage
void Agent::doForage(FastNoiseLite landscape, const float t_) {
    float energy_here = (landscape.GetNoise(x, y, t_));
    energy +=  energy_here; //(energy_here < 0.f ? 0.f : energy_here);
}

/* population level functions */
/// population moves about and forages
void popMoveForage(std::vector<Agent>& pop, FastNoiseLite landscape,
                   const float t_) {
    for(auto& indiv : pop) {
        indiv.doMove(landscape, t_);
        indiv.doForage(landscape, t_);
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
    std::vector<double> vecFitness (pop.size());
    for (size_t a = 0; a < pop.size(); a++)
    {
        vecFitness[a] = (static_cast<double> (pop[a].energy));
    }
    normaliseFitness(vecFitness);

    // weighted lottery
    std::discrete_distribution<> weightedLottery(vecFitness.begin(), vecFitness.end());

    // create new population
    std::vector<Agent> tmpPop(pop.size(), Agent(1, 1.f));
//    popRandomPos(tmpPop, 100.0);

    // assign parents
    for (size_t a = 0; a < pop.size(); a++) {

        size_t idParent = static_cast<size_t> (weightedLottery(rng));

        // replicate ANN
        tmpPop[a].annMove = pop[idParent].annMove;
        // get parent position
        tmpPop[a].x = pop[idParent].x;
        tmpPop[a].y = pop[idParent].y;
        tmpPop[a].strategy = pop[idParent].strategy;
        tmpPop[a].energy = 0.0000000001f;

        // mutate ann and strategy
        for (auto& w : tmpPop[a].annMove) {
            // probabilistic mutation of ANN
            if (rndBnl(rng)) {
                w += static_cast<float> (std::cauchy_distribution(static_cast<double>(w), mShift)(rng));
            }
        }
        if (rndBnl(rng)) {
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
    std::vector<std::vector<float> > popWeights(pop.size());
    for(size_t indiv = 0; indiv < pop.size(); ++indiv) {
        popWeights[indiv] = pop[indiv].getAnnWeights();
    }

    return popWeights;
}

#endif // AGENT_H
