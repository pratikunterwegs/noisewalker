/// define functions for pathogen spread

#include "parameters.hpp"
#include "agent.hpp"
#include "predation.hpp"

/// function to select and impose predation avoidance cost on an individual
void popPredationRisk(std::vector<Agent> &pop, const float costPredAvoid, 
    const float predZoneRadius, bgi::rtree< value, bgi::quadratic<16> > &agentRtree) {

    // discrete distribution
    std::uniform_int_distribution<int> pickAgentPred(0, static_cast<int>(pop.size()) - 1);

    size_t focalId = pickAgentPred(rng);

    // get neigbour ids
    std::vector<int> nbrsId = pop[focalId].getNbrsId(predZoneRadius,
        pop[focalId].x, pop[focalId].y, agentRtree);

    //loop over and assign cost
    const float costPred_ = costPredAvoid / static_cast<float>(nbrsId.size() + 1.f);

    pop[focalId].energy -= costPred_;

    for(size_t i = 0; i < nbrsId.size(); i++) {
        size_t toPred = nbrsId[i];
        pop[toPred].energy -= costPred_;
    }
}
