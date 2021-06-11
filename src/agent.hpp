#pragma once

// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]

// [[Rcpp::depends(BH)]]

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
        energy(0.01f),
        x(0.f),
        y(0.f),
        actv(0.f),
        resp(0.f)

    {}
    ~Agent() {}

    // agent attributes
    float energy, x, y, actv, resp;
    
    // agent functions
    float pickAngle(FastNoiseLite noise, const float perception, const int nDirections,
                    const float costSensing);
    void doSenseMove(FastNoiseLite noise, const float perception, const int directions,
        const float landsize,
        const float costSensing, const float costMove);
    void doForage(FastNoiseLite landscape, const float clamp);
    void doCompete(const float perception, bgi::rtree< value, bgi::quadratic<16> > agentRtree,
                    const float costCompete);
    
    void randomTraits();
    void randomPosition(const float landsize);
    
};

/// declare pop level functions
void popRandomPos(std::vector<Agent> &pop, const float landsize);

void popRandomTraits(std::vector<Agent> &pop);

void popMoveForageCompete(std::vector<Agent> &pop, FastNoiseLite noise, const float perception,
                          const int directions, const float landsize, const float clamp,
                          const float costMove, const float costSensing, const float costCompete);

void normaliseFitness(std::vector<double> &vecFitness);

void doReproduce(std::vector<Agent>& pop, const float landsize);

/// function to make Rtree from population
bgi::rtree< value, bgi::quadratic<16> > makeRtree (std::vector<Agent> &pop);
