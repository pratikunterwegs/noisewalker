
#' Scale weights, TANH transform, cut, and return numeric.
#'
#' @param x The values to handle.
#' @param scale The factor by which to multiply raw weights.
#' @param min_w_val The max value.
#' @param max_w_val The min value.
#' @param steps The steps between max and min values.
#'
#' @return The LOWER value of the cut bins.
#' @export
#'
cut_wt_lower = function(x, scale = 20, 
                        min_w_val = -1.01,
                        max_w_val = 1.01,
                        steps = 50) {
  x = x * scale
  x = tanh(x)
  x_class = cut(
    x, 
    seq(min_w_val, max_w_val, length.out = steps),
    right = TRUE
  )
  x_lower = as.numeric(
    stringi::stri_extract_last(x_class, regex = "[-0-9]+\\.\\d{2}")
  )
  
  x_lower
}
