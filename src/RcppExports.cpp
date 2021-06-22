// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppGSL.h>
#include <Rcpp.h>

using namespace Rcpp;

// getLandscape
Rcpp::NumericMatrix getLandscape(const float landsize, const int octaves, const float frequency, const float increment, const float clamp);
RcppExport SEXP _noisewalker_getLandscape(SEXP landsizeSEXP, SEXP octavesSEXP, SEXP frequencySEXP, SEXP incrementSEXP, SEXP clampSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const float >::type landsize(landsizeSEXP);
    Rcpp::traits::input_parameter< const int >::type octaves(octavesSEXP);
    Rcpp::traits::input_parameter< const float >::type frequency(frequencySEXP);
    Rcpp::traits::input_parameter< const float >::type increment(incrementSEXP);
    Rcpp::traits::input_parameter< const float >::type clamp(clampSEXP);
    rcpp_result_gen = Rcpp::wrap(getLandscape(landsize, octaves, frequency, increment, clamp));
    return rcpp_result_gen;
END_RCPP
}
// run_noisewalker
Rcpp::List run_noisewalker(const int popsize, const int genmax, const int timesteps, const float perception, const int directions, const float costMove, const float costCompete, const int nOctaves, const double frequency, const float landsize, const float clamp);
RcppExport SEXP _noisewalker_run_noisewalker(SEXP popsizeSEXP, SEXP genmaxSEXP, SEXP timestepsSEXP, SEXP perceptionSEXP, SEXP directionsSEXP, SEXP costMoveSEXP, SEXP costCompeteSEXP, SEXP nOctavesSEXP, SEXP frequencySEXP, SEXP landsizeSEXP, SEXP clampSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type popsize(popsizeSEXP);
    Rcpp::traits::input_parameter< const int >::type genmax(genmaxSEXP);
    Rcpp::traits::input_parameter< const int >::type timesteps(timestepsSEXP);
    Rcpp::traits::input_parameter< const float >::type perception(perceptionSEXP);
    Rcpp::traits::input_parameter< const int >::type directions(directionsSEXP);
    Rcpp::traits::input_parameter< const float >::type costMove(costMoveSEXP);
    Rcpp::traits::input_parameter< const float >::type costCompete(costCompeteSEXP);
    Rcpp::traits::input_parameter< const int >::type nOctaves(nOctavesSEXP);
    Rcpp::traits::input_parameter< const double >::type frequency(frequencySEXP);
    Rcpp::traits::input_parameter< const float >::type landsize(landsizeSEXP);
    Rcpp::traits::input_parameter< const float >::type clamp(clampSEXP);
    rcpp_result_gen = Rcpp::wrap(run_noisewalker(popsize, genmax, timesteps, perception, directions, costMove, costCompete, nOctaves, frequency, landsize, clamp));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_noisewalker_getLandscape", (DL_FUNC) &_noisewalker_getLandscape, 5},
    {"_noisewalker_run_noisewalker", (DL_FUNC) &_noisewalker_run_noisewalker, 11},
    {NULL, NULL, 0}
};

RcppExport void R_init_noisewalker(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
