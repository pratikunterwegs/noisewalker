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
    std::vector<std::vector<double> > dist_moved;
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

std::vector<double> getPopStrategyDistance(std::vector<Agent> &pop) {
    std::vector<double> vMoved (5, 0.0);
    std::vector<int> vCount (5, 0);
    for(size_t i = 0; i < pop.size(); i++) {
      vCount[pop[i].strategy]++;
      vMoved[pop[i].strategy] += pop[i].moved;
    }
    for(size_t i= 0; i < vMoved.size(); i++) {
        vMoved[i] = vMoved[i] / static_cast<double>(vCount[i]);
    }
    return vMoved;
}

void genData::updateGenData (std::vector<Agent> &pop, const int g) {

    std::vector<double> strategyProp = getPopStrategyProp(pop);
    std::vector<double> strategyMoved = getPopStrategyDistance(pop);
    std::vector<int> stratvec = {0, 1, 2, 3, 4};

    gen.push_back(std::vector<int> (5, g));
    strategy.push_back(stratvec);
    prop.push_back(strategyProp);
    dist_moved.push_back(strategyMoved);
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
                    Named("prop") = prop[li],
                    Named("mean_distance") = dist_moved[li]));
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
