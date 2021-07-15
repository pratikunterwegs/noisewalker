# function to add generation to data

#' Process Rcpp output.
#'
#' @param obj The Rcpp output object, usually a list.
#'
#' @return A dataframe of individuals and weight values.
#' @export
#'
handle_rcpp_out = function(obj) {
  
  # check for object names
  assertthat::assert_that(
    all(c("pop_data", "gens") %in% names(obj)),
    msg = "expected names not in output"
  )
  
  # get data
  data = Map(function(df, g) {
    df$gen = g
    df
  }, obj$pop_data, obj$gens)
  data = rbindlist(data)
}
