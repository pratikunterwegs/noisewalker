#include "agent.hpp"
#include "parameters.hpp"
#include <boost/foreach.hpp>

/// random number generator
std::mt19937 rng;

/// function to update population Rtree
bgi::rtree< value, bgi::quadratic<16> > makeRtree (std::vector<Agent> &pop) {
    // initialise rtree
    bgi::rtree< value, bgi::quadratic<16> > tmpRtree;
    for (size_t i = 0; i < pop.size(); ++i)
    {
        point p = point(pop[i].x, pop[i].y);
        tmpRtree.insert(std::make_pair(p, i));
    }
    return tmpRtree;
}


/// random trait value dist
std::uniform_real_distribution<float> randTrait(0.f, 1.f);

/// initialise the population at random positions
void Agent::randomPosition(const float landsize) {
    x = randTrait(rng) * static_cast<double>(landsize);
    y = randTrait(rng) * static_cast<double>(landsize);
}

/// initialise with random position
void popRandomPos(std::vector<Agent> &pop, const float landsize) {
  for(auto& indiv : pop) {
    indiv.randomPosition(landsize);
  }
}

/// pick random traits
void Agent::randomTraits() {
    actv = randTrait(rng);
    resp = randTrait(rng);
}

/// pop level random traits
void popRandomTraits(std::vector<Agent> &pop) {
  for(auto& indiv : pop) {
    indiv.randomTraits();
  }
}

/// random angle distribution
std::uniform_real_distribution<float> randAngle(0.f, 2.f * M_PI);

/// function to get best angle
float Agent::pickAngle(FastNoiseLite noise, const float perception,
    const int nDirections, const float costSensing) {
    
    float moveAngle = 0.f;
    std::bernoulli_distribution randSense(resp);
    if (randSense(rng)) {
        float twopi = 2.f * M_PI;
        // what increment for nDirections samples in a circle around the agent
        float increment = twopi / static_cast<float>(nDirections);
        float sampleX, sampleY, foodHere;
        // cycle through samples and check for angle with max resources
        for(float theta = 0.f; theta < twopi - increment; theta += increment) {

            sampleX = x + (perception * static_cast<float>(cos(theta)));
            sampleY = y + (perception * static_cast<float>(sin(theta)));

            foodHere = noise.GetNoise(sampleX, sampleY);

            if (foodHere > moveAngle) {
                moveAngle = theta;
            }
        }

        // subtract sensory cost
        energy -= costSensing;
    }
    else {
        moveAngle = randAngle(rng);
    }

    return moveAngle;
}

/// agent function to choose a new position
void Agent::doSenseMove(FastNoiseLite noise, const float perception, const int directions, const float landsize,
    const float costSensing, const float costMove) {
    
    // agents sense based on responsiveness
    float moveAngle = pickAngle(noise, perception, directions);

    // agents move based on activity
    std::bernoulli_distribution randMove(actv);
    if (randMove(rng)) {
        // get new position
        x = x + (perception * static_cast<float>(cos(moveAngle)));
        y = y + (perception * static_cast<float>(sin(moveAngle)));

        // crudely wrap movement
        if(x > landsize | x < 0.f) {
            x = std::fabs(std::fmod(x, landsize));
        }
        if(y > landsize | y < 0.f) {
            y = std::fabs(std::fmod(y, landsize));
        }

        energy -= costMove;
    }
}

/// agent function to forage
void Agent::doForage(FastNoiseLite landscape, const float clamp) {
    float energy_here = (landscape.GetNoise(x, y));
    // the clamp is defined in parameters.hpp
    energy +=  (energy_here < clamp ? 0.f : energy_here);
}

/// function to count neighbours
void Agent::doCompete(const float perception, 
    bgi::rtree< value, bgi::quadratic<16> > agentRtree) {

    // work in progress
    std::vector<int> agentId;
    std::vector<value> nearAgents;
    
    // std::cout << "id = " << id << " at " << bg::wkt<point> (point(coordX[id], coordY[id])) << "\n";

    // query for a simple box
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(x, y)) < perception;}),
        std::back_inserter(nearAgents));

    BOOST_FOREACH(value const& v, nearAgents) {
        // std::cout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
        agentId.push_back(v.second);
    }

    nearAgents.clear();

    // std::cout << "near agents = " << agentId.size() << "\n\n";

    // subtract energy per neighbour within range
    energy -= (static_cast<float> (agentId.size()) * costCompete);
    
}

/* population level functions */
/// population moves about and forages
void popMoveForageCompete(std::vector<Agent>& pop, FastNoiseLite noise,
    const float perception, const int directions, const float clamp) {
    for(auto& indiv : pop) {
        indiv.doSenseMove(noise, perception, directions);
        indiv.doForage(noise, clamp);
    }

    // first update Rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree = makeRtree(pop);

    // all moves done now count neighbours
    for(auto& indiv : pop) {
        indiv.doCompete(perception, agentRtree);
    }
}

/// minor function to normalise vector
// THIS IS AN ISSUE IF YOU WANT TO RETURN ENERGY PER GEN AS ENERGY IS NORMED
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

/// make cauchy distribution
std::bernoulli_distribution randMutEvent(mutProb);
std::cauchy_distribution<float> randMutSize(0.f, mutSize);

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
    std::vector<Agent> tmpPop(pop.size());

    // assign parents
    for (size_t a = 0; a < pop.size(); a++) {

        size_t idParent = static_cast<size_t> (weightedLottery(rng));
        // get parent position
        tmpPop[a].x = pop[idParent].x;
        tmpPop[a].y = pop[idParent].y;
        tmpPop[a].actv = pop[idParent].actv;
        tmpPop[a].resp = pop[idParent].resp;
        tmpPop[a].energy = 0.001f;
    }

    // mutation
    for(auto& indiv : tmpPop) {
        if (randMutEvent(rng)) {
            indiv.actv += randMutSize(rng);

            // cap at 0 and 1
            if (indiv.actv > 1.f) indiv.actv = 1.f;
            if (indiv.actv < 0.f) indiv.actv = 0.f; 
        }
        if (randMutEvent(rng)) {
            if (indiv.resp > 1.f) indiv.resp = 1.f;
            if (indiv.resp < 0.f) indiv.resp = 0.f;
        }
    }

    // swap tmp pop for pop
    std::swap(pop, tmpPop);
    // randomise position?
    popRandomPos(tmpPop, landsize);
    tmpPop.clear();
}
