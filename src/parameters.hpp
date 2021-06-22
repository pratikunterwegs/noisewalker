#pragma once
// Enable C++14 via this plugin to suppress 'long long' errors
// [[Rcpp::plugins("cpp14")]]

// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppGSL)]]
#include <Rcpp.h>

#include <random>

// mutation params
const float mutProb = 0.01f;
const float mutSize = 0.001f;

// make a gsl rng and a regular rng
extern std::mt19937 rng;
