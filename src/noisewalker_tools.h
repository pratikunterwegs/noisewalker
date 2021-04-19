#ifndef NOISEWALKER_TOOLS_H
#define NOISEWALKER_TOOLS_H

#include <iostream>
#include <fstream>
#include <vector>
#include "agent.h"
#include <cmath>
#include <algorithm> 
#include <cassert>
#include <Rcpp.h>

using namespace Rcpp;
struct genData {
public:
    std::vector<std::vector<int> > gen;
    std::vector<std::vector<int> > strategy;
    std::vector<std::vector<double> > prop;
    void updateGenData (std::vector<Agent> &pop, const int gen);
    Rcpp::List returnGenData ();
};

void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    std::vector<double> strategyProp = getPopStrategyProp(pop);
    std::vector<int> stratvec = {0, 1, 2};

    gen.push_back(std::vector<int> {g, g, g});
    strategy.push_back(stratvec);
    prop.push_back(strategyProp);
}

Rcpp::List genData::returnGenData() {
    assert(gen.size() > 0 && "poor gen size");
    Rcpp::List genDataList;

    for(size_t li = 0; li < gen.size(); li++) {
        genDataList.push_back(Rcpp::DataFrame::create(
                    Named("gen") = gen[li],
                    Named("strategy") = strategy[li],
                    Named("prop") = prop[li]));
    }

    return genDataList;
}

#endif // NOISEWALKER_TOOLS_H
