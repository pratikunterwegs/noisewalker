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

struct fitnessData {
public:
    std::vector<int> gen;
    std::vector<double> meanFitness;
    void updateFitnessData(std::vector<Agent> &pop, const int gen);
    Rcpp::List returnFitnessData ();
};

struct moveData {
public:
    std::vector<double> moved;
    std::vector<int> strategies;

    void updateMoveData(std::vector<Agent> &pop);
    Rcpp::DataFrame returnMoveData();
};

void moveData::updateMoveData (std::vector<Agent> &pop) {
    for (size_t i = 0; i < pop.size(); i++) {
        moved.push_back(pop[i].moved);
        strategies.push_back(pop[i].strategy);
    }
    assert(strategies.size() == pop.size() && "move data not correctly made");
}

Rcpp::DataFrame moveData::returnMoveData() {
    return Rcpp::DataFrame::create(
        Named("strategy") = strategies,
        Named("moved") = moved
    );
}

/// function to count strategy proportions
std::vector<double> getPopStrategyProp (std::vector<Agent> &pop) {
  std::vector<int> vCount (5, 0);
  for(size_t i = 0; i < pop.size(); i++) {
    vCount[pop[i].strategy]++;
  }
  std::vector<double> vProp (5, 0.0);
  for(size_t i = 0; i < vCount.size(); i++) {
    vProp[i] = static_cast<double> (vCount[i]) / static_cast<double> (pop.size());
  }
  return vProp;
}

void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    std::vector<double> strategyProp = getPopStrategyProp(pop);
    std::vector<int> stratvec = {0, 1, 2, 3, 4};

    gen.push_back(std::vector<int> (5, g));
    strategy.push_back(stratvec);
    prop.push_back(strategyProp);
}

void fitnessData::updateFitnessData(std::vector<Agent> &pop, const int g) {
    gen.push_back(g);
    double meanFitness_ = 0.0;
    for(size_t i = 0; i < pop.size(); i++) {
        meanFitness_ += pop[i].energy;
    }
    meanFitness.push_back(meanFitness_/static_cast<double>(pop.size()));
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

Rcpp::List fitnessData::returnFitnessData() {
    assert(gen.size() > 0 && "poor gen size");
    Rcpp::List fitnessDataList;
    for(size_t li = 0; li < gen.size(); li++) {
        fitnessDataList.push_back(Rcpp::DataFrame::create(
                    Named("gen") = gen[li],
                    Named("mean_fitness") = meanFitness[li]));
    }
    return fitnessDataList;
}

#endif // NOISEWALKER_TOOLS_H
