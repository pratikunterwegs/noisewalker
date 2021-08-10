#' Get optimal group sizes.
#'
#' @param data The trait dataset
#' @param coef_agents The agent preference.
#' @param coef_agents_sq The agent count squared preference.
#' @return A vector of optimal group sizes
#' @export
#'
optimal_group_size = function(data, 
    coef_agents, coef_agents_sq) {
  
  # formula for the X-coord of the axis of symmetry of a parabola
  # -b/(2a)
  - data[[coef_agents_sq]] / (2 * data[[coef_agents]])

}
