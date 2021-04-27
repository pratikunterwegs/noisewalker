// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// run_noisewalker
Rcpp::List run_noisewalker(const int popsize, const float percep_range, const int genmax, const int timesteps, const float t_increment, const int nOctaves, const double frequency, const double landsize);
RcppExport SEXP _noisewalker_run_noisewalker(SEXP popsizeSEXP, SEXP percep_rangeSEXP, SEXP genmaxSEXP, SEXP timestepsSEXP, SEXP t_incrementSEXP, SEXP nOctavesSEXP, SEXP frequencySEXP, SEXP landsizeSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type popsize(popsizeSEXP);
    Rcpp::traits::input_parameter< const float >::type percep_range(percep_rangeSEXP);
    Rcpp::traits::input_parameter< const int >::type genmax(genmaxSEXP);
    Rcpp::traits::input_parameter< const int >::type timesteps(timestepsSEXP);
    Rcpp::traits::input_parameter< const float >::type t_increment(t_incrementSEXP);
    Rcpp::traits::input_parameter< const int >::type nOctaves(nOctavesSEXP);
    Rcpp::traits::input_parameter< const double >::type frequency(frequencySEXP);
    Rcpp::traits::input_parameter< const double >::type landsize(landsizeSEXP);
    rcpp_result_gen = Rcpp::wrap(run_noisewalker(popsize, percep_range, genmax, timesteps, t_increment, nOctaves, frequency, landsize));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_noisewalker_run_noisewalker", (DL_FUNC) &_noisewalker_run_noisewalker, 8},
    {NULL, NULL, 0}
};

RcppExport void R_init_noisewalker(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
