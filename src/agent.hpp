#pragma once

#define _USE_MATH_DEFINES
/// code to make agents
#include <vector>
#include <cassert>

#include <Rcpp.h>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

using namespace Rcpp;
#include "parameters.hpp"
#include "FastNoiseLite.h"

// boost geometry types
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

// Agent class
class Agent {
public:
    Agent() :
        // count agents correctly heritable parameter is s_range
        energy(1e-5f),
        x(0.f),
        y(0.f),
        coefFood(0.f),
        coefNbrs(0.f),
        coefRisk(0.f)

    {}
    ~Agent() {}

    // agent attributes
    float energy, x, y, coefFood, coefNbrs, coefRisk;
    
    // agent functions
    int countNbrsAt(const float perception,
                    const float xloc, const float yloc,
                    bgi::rtree< value, bgi::quadratic<16> > &agentRtree);
    void doSenseMove(FastNoiseLite &noise, 
                    FastNoiseLite &risk, const float t_, const float perception,
                    const int directions, const float landsize,
                    bgi::rtree< value, bgi::quadratic<16> > &agentRtree, const float costMove);
    void doEnergetics(FastNoiseLite &noise, FastNoiseLite &risk, 
                    bgi::rtree< value, bgi::quadratic<16> > &agentRtree,
                    const float perception,
                    const float t_, const float clamp);
    
    void randomTraits();
    void randomPosition(const float landsize);
    
};

/// declare pop level functions
void popRandomPos(std::vector<Agent> &pop, const float landsize);

void popRandomTraits(std::vector<Agent> &pop);

void popMoveForageCompete(std::vector<Agent>& pop, FastNoiseLite &noise,
    FastNoiseLite &risk,
    const float t_,
    const float perception, const int directions, 
    const float landsize, const float clamp,
    const float costMove);

void handleFitness(std::vector<float> &vecFitness);

void doReproduce(std::vector<Agent>& pop, const float landsize);

/// function to make Rtree from population
bgi::rtree< value, bgi::quadratic<16> > makeRtree (std::vector<Agent> &pop);
