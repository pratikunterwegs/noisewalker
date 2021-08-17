/// define functions for pathogen spread

#include "parameters.hpp"
#include "agent.hpp"
#include "pathospread.hpp"
#include <unordered_set>

/// function to infect n individuals
void popIntroPathogen(std::vector<Agent> &pop, const int nInfected) {

    // discrete distribution
    std::uniform_int_distribution<int> pickAgent(0, static_cast<int>(pop.size()) - 1);

    // sample the population indices without replacement
    // using an unordered set
    std::unordered_set<int> infectedAgents;

    // loop through the intended number of infections
    for (int i = 0; i < nInfected;)
    {
        // sample an index
        int infectedAgentIndex = pickAgent(rng);
        if(infectedAgents.find(infectedAgentIndex) == infectedAgents.end()) {
            
            // add agent to infected agents
            infectedAgents.insert(infectedAgentIndex);

            // check this agent is not already infected
            assert(!pop[infectedAgentIndex].infected && 
                "intro pathogen: agent is already infected, error in fun");

            // toggle infected agents boolean for infected
            pop[infectedAgentIndex].infected = true;
            pop[infectedAgentIndex].timeInfected = 0;
            i++;
        }
    }
}

/// function to spread pathogen
void popPathogenSpread(std::vector<Agent> &pop, const float perception,
                       const float pTransmit, const int t_,
                       bgi::rtree< value, bgi::quadratic<16> > &agentRtree) {

    // bernoulli distribution of transmission
    std::bernoulli_distribution pathogenTransmits(pTransmit);

    // looping through agents, query rtree for neighbours
    for (size_t i = 0; i < pop.size(); i++)
    {
        // spread to neighbours if self infected
        if (pop[i].infected) 
        {
            // get neigbour ids
            std::vector<int> nbrsId = pop[i].getNbrsId(perception,
                pop[i].x, pop[i].y, agentRtree);

            if (nbrsId.size() > 0) 
            {
                // loop through neighbours
                for(size_t j = 0; j < nbrsId.size(); j++) 
                {
                    size_t toInfect = nbrsId[j];
                    if (!pop[toInfect].infected) 
                    {
                        // infect neighbours with prob p
                        if(pathogenTransmits(rng))
                        {
                            pop[toInfect].infected = true;
                            pop[toInfect].timeInfected = t_;
                        }
                    }
                }
            }
        }   
    }
}

/// function for pathogen cost
void popPathogenCost(std::vector<Agent> &pop, const float costInfection, const int t_) {
    for (size_t i = 0; i < pop.size(); i++)
    {
        if(pop[i].infected) {
            pop[i].energy -= (costInfection * static_cast<float>(t_ - pop[i].timeInfected));
        }
    }
}
