#include "agent.hpp"
#include "datatypes.hpp"

/// function to update generation data
void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    // make vector of activities
    std::vector<float> vecCoefFood (pop.size(), 0.f);
    std::vector<float> vecCoefNbrs (pop.size(), 0.f);
    std::vector<float> vecCoefRisk (pop.size(), 0.f);
    std::vector<float> vecEnrg (pop.size(), 1e-5f);
    std::vector<float> vecMoved (pop.size(), 0.f);
    std::vector<int> vecTimeInfc (pop.size(), 0);
    std::vector<int> vecInteractions (pop.size(), 0);
    
    for (size_t i = 0; i < pop.size(); i++)
    {
        vecCoefFood[i] = pop[i].coefFood;
        vecCoefNbrs[i] = pop[i].coefNbrs;
        vecCoefRisk[i] = pop[i].coefRisk;
        // Rcpp::Rcout << "raw energy total recorded in data = " << pop[i].energy << "\n";
        vecEnrg[i] = pop[i].energy;
        vecMoved[i] = pop[i].moved;
        vecTimeInfc[i] = pop[i].timeInfected;
        vecInteractions[i] = pop[i].nbrs;
    }    
    // add to data
    gen.push_back(g);
    genCoefFood.push_back(vecCoefFood);
    genCoefNbrs.push_back(vecCoefNbrs);
    genCoefRisk.push_back(vecCoefRisk);
    genEnergy.push_back(vecEnrg); // return absolute, not transformed energy
    genMoved.push_back(vecMoved);
    genTimeInfc.push_back(vecTimeInfc);
    genInteractions.push_back(vecInteractions);
}

/// function to get all generations data
Rcpp::List genData::returnGenData() {
    assert(gen.size() > 0 && "poor gen size");
    // make Rcpp list
    Rcpp::List genDataList (gen.size());

    // loop through stored data
    for (size_t i = 0; i < gen.size(); i++)
    {
        genDataList[i] = DataFrame::create(
            Named("coef_food") = genCoefFood[i],
            Named("coef_nbrs") = genCoefNbrs[i],
            Named("coef_risk") = genCoefRisk[i],
            Named("energy") = genEnergy[i],
            Named("moved") = genMoved[i],
            Named("time_infected") = genTimeInfc[i],
            Named("interactions") = genInteractions[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gen
    );

    return dataToReturn;
}

/// function to update generation data
void posData::updateGenData (std::vector<Agent> &pop, const int g) {

    // make vector of activities
    std::vector<float> vecX (pop.size(), 0.f);
    std::vector<float> vecY (pop.size(), 0.f);
    
    for (size_t i = 0; i < pop.size(); i++)
    {
        vecX[i] = pop[i].x;
        vecY[i] = pop[i].y;
    }    
    // add to data
    gen.push_back(g);
    genX.push_back(vecX);
    genY.push_back(vecY);
}
