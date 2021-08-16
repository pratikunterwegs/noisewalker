/// functions to infect an agent and spread disease
#pragma once

#include <vector>
#include <algorithm> 
#include <cassert>

#include <Rcpp.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "agent.hpp"

/// function to select and impose predation avoidance cost on an individual
void popPredationRisk(std::vector<Agent> &pop, const float costPredAvoid, 
    const float predZoneRadius, bgi::rtree< value, bgi::quadratic<16> > &agentRtree);
