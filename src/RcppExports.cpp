// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// run_noisewalker
void run_noisewalker(const int genmax, const int timesteps, const int nOctaves, const double frequency, const double newSrange, const std::string rep);
RcppExport SEXP _noisewalker_run_noisewalker(SEXP genmaxSEXP, SEXP timestepsSEXP, SEXP nOctavesSEXP, SEXP frequencySEXP, SEXP newSrangeSEXP, SEXP repSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type genmax(genmaxSEXP);
    Rcpp::traits::input_parameter< const int >::type timesteps(timestepsSEXP);
    Rcpp::traits::input_parameter< const int >::type nOctaves(nOctavesSEXP);
    Rcpp::traits::input_parameter< const double >::type frequency(frequencySEXP);
    Rcpp::traits::input_parameter< const double >::type newSrange(newSrangeSEXP);
    Rcpp::traits::input_parameter< const std::string >::type rep(repSEXP);
    run_noisewalker(genmax, timesteps, nOctaves, frequency, newSrange, rep);
    return R_NilValue;
END_RCPP
}
// get_values_1d
Rcpp::NumericVector get_values_1d(const int nOctaves, const double frequency, const double increment, const int nValues);
RcppExport SEXP _noisewalker_get_values_1d(SEXP nOctavesSEXP, SEXP frequencySEXP, SEXP incrementSEXP, SEXP nValuesSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type nOctaves(nOctavesSEXP);
    Rcpp::traits::input_parameter< const double >::type frequency(frequencySEXP);
    Rcpp::traits::input_parameter< const double >::type increment(incrementSEXP);
    Rcpp::traits::input_parameter< const int >::type nValues(nValuesSEXP);
    rcpp_result_gen = Rcpp::wrap(get_values_1d(nOctaves, frequency, increment, nValues));
    return rcpp_result_gen;
END_RCPP
}
// get_rand_values
Rcpp::NumericVector get_rand_values(const int nValues, const double increment);
RcppExport SEXP _noisewalker_get_rand_values(SEXP nValuesSEXP, SEXP incrementSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const int >::type nValues(nValuesSEXP);
    Rcpp::traits::input_parameter< const double >::type increment(incrementSEXP);
    rcpp_result_gen = Rcpp::wrap(get_rand_values(nValues, increment));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_noisewalker_run_noisewalker", (DL_FUNC) &_noisewalker_run_noisewalker, 6},
    {"_noisewalker_get_values_1d", (DL_FUNC) &_noisewalker_get_values_1d, 4},
    {"_noisewalker_get_rand_values", (DL_FUNC) &_noisewalker_get_rand_values, 2},
    {NULL, NULL, 0}
};

RcppExport void R_init_noisewalker(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}