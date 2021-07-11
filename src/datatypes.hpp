#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm> 
#include <cassert>
#include <Rcpp.h>
#include "agent.hpp"

using namespace Rcpp;
struct genData {
public:
    std::vector<int> gen;
    std::vector<std::vector<float> > genCoefFood;
    std::vector<std::vector<float> > genCoefNbrs;
    std::vector<std::vector<float> > genCoefRisk;
    std::vector<std::vector<float> > genEnergy;
    std::vector<std::vector<float> > genMoved;
    std::vector<std::vector<float> > genTimeInfc;
    void updateGenData (std::vector<Agent> &pop, const int gen);
    Rcpp::List returnGenData ();
};
