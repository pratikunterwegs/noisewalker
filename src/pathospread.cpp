/// define functions for pathogen spread

#include "agent.hpp"
#include "pathospread.hpp"

/// random number generator
std::mt19937 rng;

/// function to infect n individuals
void popIntroPathogen(std::vector<Agent> &pop, const int nInfected) {

    // discrete distribution
    std::uniform_int_distribution<> pickAgent(0, pop.size());

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
                       const float pTransmit, const int t_) {

    // bernoulli distribution of transmission
    std::bernoulli_distribution<> pathogenTransmits(pTransmit);
    
    // make Rtree
    bgi::rtree< value, bgi::quadratic<16> > agentRtree;
    if (allow_compete) {
        agentRtree = makeRtree(pop);
    }

    // looping through agents, query rtree for neighbours
    for (size_t i = 0; i < pop.size(); i++)
    {
        // spread to neighbours if self infected
        if (pop[i].infected) 
        {
            // get neigbour ids
            std::vector<int> nbrsId = getNbrsId(perception,
                pop[i].x, pop[i].y, agentRtree);

            if (nbrsId.size() > 0) 
            {
                // loop through neighbours
                for(size_t j = 0; j < nbrsId.size(); j++) 
                {
                    if (!pop[j].infected) 
                    {
                        // infect neighbours with prob p
                        if(pathogenTransmits(pTransmit)) 
                        {
                            pop[j].infected = true;
                            pop[j].timeInfected = t_;
                        }
                    }
                }
            }
        }   
    }
}
