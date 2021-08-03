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
std::uniform_real_distribution<float> randTrait(-0.1f, 0.1f);

// normal distr around 0
std::normal_distribution<float> normDist(0.0f, 0.1f);

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
    coefFood = randTrait(rng);
    coefNbrs = randTrait(rng);
    coefRisk = randTrait(rng);
}

/// pop level random traits
void popRandomTraits(std::vector<Agent> &pop) {
  for(auto& indiv : pop) {
    indiv.randomTraits();
  }
}

/// function to count neighbours
int Agent::countNbrsAt(const float perception, 
    const float xloc, const float yloc,
    bgi::rtree< value, bgi::quadratic<16> > &agentRtree) {
    // work in progress
    int nNbrs = 0;
    std::vector<value> nearAgents;
    
    // std::cout << "id = " << id << " at " << bg::wkt<point> (point(coordX[id], coordY[id])) << "\n";

    // query for a simple box
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < perception;}),
        std::back_inserter(nearAgents));

    BOOST_FOREACH(value const& v, nearAgents) {
        // std::cout << bg::wkt<point> (v.first) << " - " << v.second << "\n";
        // subtract energy per neighbour within range
        nNbrs ++;
    }
    nearAgents.clear();
    return nNbrs;
}

/// function to get neighbour identities
std::vector<int> Agent::getNbrsId(const float perception, 
    const float xloc, const float yloc,
    bgi::rtree< value, bgi::quadratic<16> > &agentRtree) {
    // work in progress
    int nNbrs = 0;
    std::vector<value> nearAgents;

    std::vector<int> nearAgentId;
    
    // query for a simple box
    agentRtree.query(bgi::satisfies([&](value const& v) {
        return bg::distance(v.first, point(xloc, yloc)) < perception;}),
        std::back_inserter(nearAgents));

    BOOST_FOREACH(value const& v, nearAgents) {
        nearAgentId.push_back(v.second);
    }
    nearAgents.clear();

    return nearAgentId;
}

/// agent function to choose a new position
void Agent::doSenseMove(FastNoiseLite &noise, 
    const float t_, const float perception,
    const int directions, 
    const float clamp, const float landsize,
    bgi::rtree< value, bgi::quadratic<16> > &agentRtree, const float costMove,
    const bool allow_compete) {
    
    // set default values --- stay in place
    float newX = x; float newY = y;
    float foodHere = noise.GetNoise(newX, newY, static_cast<float>(t_));

    // food here is 0 if less than clamp (0.f)
    foodHere = foodHere > clamp ? foodHere : 0.f;
    float nbrsHere = allow_compete ? static_cast<float>(countNbrsAt(perception, newX, newY, agentRtree)) : 0.f;

    // suitability at location is calculated as having risk = 0.0
    // that is, no risk is perceived here.
    float best_suit = (coefFood * foodHere) + (coefNbrs * nbrsHere) + (coefRisk * (nbrsHere * nbrsHere));
    
    float twopi = 2.f * M_PI;
    // what increment for nDirections samples in a circle around the agent
    float increment = twopi / static_cast<float>(directions);
    float sampleX, sampleY;
    // cycle through samples and check for angle with max resources
    for(float theta = 0.f; theta < twopi - increment; theta += increment) {

        sampleX = x + (perception * static_cast<float>(cos(theta)));
        sampleY = y + (perception * static_cast<float>(sin(theta)));

        // agent senses 0.f if food is less than clamp
        foodHere = noise.GetNoise(sampleX, sampleY, static_cast<float>(t_));
        foodHere = foodHere > clamp ? foodHere : 0.f;

        nbrsHere = allow_compete ? static_cast<float>(countNbrsAt(perception, sampleX, sampleY, agentRtree)) : 0.f;

        // suitability at new location is modified by individual's perceived risk
        float new_suit = (coefFood * foodHere) + (coefNbrs * nbrsHere) + (coefRisk * (nbrsHere * nbrsHere));

        if (new_suit > best_suit) {
            newX = sampleX;
            newY = sampleY;
            // best_suit = new_suit;
            break; // chose the new location if 
        }
    }

    // subtract cost of movement if any
    // comparing floats is difficult so determine if moved distance is
    // more than 1% of the perception range (default movement distance)
    float dist = std::sqrt(std::pow(x - newX, 2.0) + std::pow(y - newY, 2.0));
    if(dist > perception / 1e2f) {
        energy -= costMove;
        moved += perception; // add distance moved
    }

    // get new position
    x = newX; y = newY;

    // crudely wrap movement
    if((x > landsize) | (x < 0.f)) {
        x = std::fabs(std::fmod(x, landsize));
    }
    if((y > landsize) | (y < 0.f)) {
        y = std::fabs(std::fmod(y, landsize));
    }
}

/// agent function to forage
void Agent::doEnergetics(FastNoiseLite &noise, 
    bgi::rtree< value, bgi::quadratic<16> > &agentRtree,
    const float perception,
    const float t_, const float clamp,
    const bool allow_compete) {
    
    // energy
    float energy_here = (noise.GetNoise(x, y, static_cast<float>(t_)));
    // Rcpp::Rcout << "energy here = " << energy_here << "\n";

    // both divided by number of neighbours
    int nNbrs = allow_compete ? countNbrsAt(perception, x, y, agentRtree) : 0;

    float nbrs_f = static_cast<float>(nbrs); // + 1.f; // to prevent divisions by 0

    // Rcpp::Rcout << "nbrs here = " << nbrs << "\n";
    energy_here = energy_here + (1.f - (1.f / nbrs_f));

    // Rcpp::Rcout << "scaled energy here = " << energy_here << "\n";
    // energetic balance
    energy +=  (energy_here < clamp ? 0.f : energy_here);
    
    // Rcpp::Rcout << "overall energy here = " << (energy_here < clamp ? 0.f : energy_here) << "\n";
}

/* population level functions */
/// population moves about and forages
void popMoveForageCompete(std::vector<Agent>& pop, FastNoiseLite &noise,
    const float t_,
    const float perception, const int directions, 
    const float landsize, const float clamp,
    const float costMove,
    const bool allow_compete) {

    // make Rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;
    if (allow_compete) {
        agentRtree = makeRtree(pop);
    }

    for(auto& indiv : pop) {
        indiv.doSenseMove(noise, t_, perception, directions, clamp, landsize, 
            agentRtree, costMove, allow_compete);
        indiv.doEnergetics(noise, agentRtree, perception, t_, clamp,
            allow_compete);
    }
}

/// minor function to handle energy
void handleFitness(std::vector<float> &vecFitness) {

    // negative energy is 0, add 1e-5 to all positive values
    for (size_t i = 0; i < vecFitness.size(); i++)
    {
        // Rcpp::Rcout << "raw energy total = " << vecFitness[i] << "\n";
        vecFitness[i] = vecFitness[i] > 0.f ? vecFitness[i] + 1e-5 : 1e-5;
        // Rcpp::Rcout << "scaled energy total = " << vecFitness[i] << "\n";
        assert(vecFitness[i] > 0.f && "Agent energy is 0!");
    }

    std::vector<float> tmpvec = vecFitness;
    std::sort(tmpvec.begin(), tmpvec.end());

    // get max fitness
    float maxFitness = tmpvec.back();

    // rescale values between 1e-5f and 1.f
    for (size_t j = 0; j < vecFitness.size(); ++j) {
        vecFitness[j] = vecFitness[j] / (maxFitness);
        assert(vecFitness[j] > 0.f && "Agent energy is 0!");
    }

}

/// make cauchy distribution
std::bernoulli_distribution randMutEvent(mutProb);
std::cauchy_distribution<float> randMutSize(0.f, mutSize);

/// pop reproduces
void doReproduce(std::vector<Agent>& pop, const float landsize) {
    // make fitness vec
    std::vector<float> vecFitness (pop.size());
    for (size_t a = 0; a < pop.size(); a++)
    {
        vecFitness[a] = pop[a].energy;
    }
    handleFitness(vecFitness);

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
        tmpPop[a].coefFood = pop[idParent].coefFood;
        tmpPop[a].coefNbrs = pop[idParent].coefNbrs;
        tmpPop[a].coefRisk = pop[idParent].coefRisk;
        tmpPop[a].energy = 1e-5f;
        tmpPop[a].nbrs = 0;
    }

    // mutation
    for(auto& indiv : tmpPop) {
        if (randMutEvent(rng)) {
            indiv.coefFood += randMutSize(rng);
        }
        if (randMutEvent(rng)) {
            indiv.coefNbrs += randMutSize(rng);
        }
        if (randMutEvent(rng)) {
            indiv.coefRisk += randMutSize(rng);
        }
    }

    // swap tmp pop for pop
    std::swap(pop, tmpPop);
    // randomise position?
    popRandomPos(tmpPop, landsize);
    tmpPop.clear();
}
