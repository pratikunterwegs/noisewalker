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
            i++;
        }
    }
}
