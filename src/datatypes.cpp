
#include "datatypes.hpp"

/// function to update generation data
void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    // make vector of activities
    std::vector<float> vecActv (pop.size(), 0.f);
    std::vector<float> vecResp (pop.size(), 0.f);
    std::vector<float> vecEnrg (pop.size(), 0.f);
    
    for (size_t i = 0; i < pop.size(); i++)
    {
        vecActv[i] = pop[i].actv;
        vecResp[i] = pop[i].resp;
        vecEnrg[i] = pop[i].energy;
    }    
    // add to data
    gen.push_back(g);
    genActv.push_back(vecActv);
    genResp.push_back(vecResp);
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
            Named("actv") = genActv[i],
            Named("resp") = genResp[i],
            Named("energy") = genEnergy[i]
        );
    }
    List dataToReturn = List::create(
        Named("pop_data") = genDataList,
        Named("gens") = gen
    );

    return dataToReturn;
}
