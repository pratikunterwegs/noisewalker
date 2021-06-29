
#include "datatypes.hpp"

/// function to update generation data
void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    // make vector of activities
    std::vector<float> vecCoefFood (pop.size(), 0.f);
    std::vector<float> vecCoefNbrs (pop.size(), 0.f);
    std::vector<float> vecCoefRisk (pop.size(), 0.f);
    std::vector<float> vecEnrg (pop.size(), 0.f);
    
    for (size_t i = 0; i < pop.size(); i++)
    {
        vecCoefFood[i] = pop[i].coefFood;
        vecCoefNbrs[i] = pop[i].coefNbrs;
        vecCoefRisk[i] = pop[i].coefRisk;
        vecEnrg[i] = pop[i].energy;
    }    
    // add to data
    gen.push_back(g);
    genCoefFood.push_back(vecCoefFood);
    genCoefNbrs.push_back(vecCoefNbrs);
    genCoefRisk.push_back(vecCoefRisk);
    genEnergy.push_back(vecEnrg);
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
            Named("energy") = genEnergy[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gen
    );

    return dataToReturn;
}
