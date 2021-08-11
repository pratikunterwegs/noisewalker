#' Get optimal group sizes.
#'
#' @param coef_agents The agent preference.
#' @param coef_agents_sq The agent count squared preference.
#' @return A vector of optimal group sizes
#' @export
#'
optimal_group_size = function(coef_agents, coef_agents_sq) {
  
  # formula for the X-coord of the axis of symmetry of a parabola
  # -b/(2a)
  - coef_agents_sq / (2 * coef_agents)

}

#' Get response to agents.
#' 
#' @param coef_agents The agent preference.
#' @param coef_agents_sq The agent count squared preference.
#' @param plot Whether to plot.
#' @return A vector of group size preferences.
#' @export
#' 
agent_response = function(coef_agents, coef_agents_sq, 
                          stepsize = 2, plot = FALSE) {

  resp = (coef_agents * (seq(from = 0, to = 20, by = stepsize))) + 
    (coef_agents_sq * (seq(from = 0, to = 20, by = stepsize))^2)

  if(plot) {
    plot(resp, type = "b", xlab = "group size", ylab = "pref", pch = 16,
         main = sprintf("coef_agent = %.4f coef_ag_sq = %.4f", coef_agents, coef_agents_sq))
  } else {
     resp
  }
}
