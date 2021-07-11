/// functions to infect an agent and spread disease
#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm> 
#include <cassert>

#include <Rcpp.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "agent.hpp"

/// function to infect n individual
void popIntroPathogen(std::vector<Agent> &pop, const int nInfected);
/// function to spread pathogen
void popPathogenSpread(std::vector<Agent> &pop, const float perception,
                       const float pTransmit);
/// function for pathogen cost
void popPathogenCost(std::vector<Agent> &pop, const float costInfection, const int t_);
