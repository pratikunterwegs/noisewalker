# check the simulation
# libraries for data and plots
library(ggplot2)
library(patchwork)
library(data.table)

# load the lib, better to restart R
library(noisewalker)

# autocorrelation structures functions
# small function for acf
get_acf_vals <- function(z, ...) {
    acfout = stats::acf(z, ..., plot = FALSE)
    data.frame(
        acfval = acfout$acf,
        acflag = acfout$lag
    )
}

# small fun for acf range below 0.2
get_acf_range <- function(z, corr_val = 0.1) {
  acfr <- which(z["acfval"] < corr_val)[1]
  if (is.na(acfr)){
    acfr <- Inf
  }
  return(acfr)
}

# prepare combinations
params = CJ(
    landsize = 10,
    octaves = 1,
    frequency = c(0.1, 0.25, 0.5, 1, 2),
    increment = 0.05,
    clamp = -1.1
)

# get landscape data
data = lapply(seq(nrow(params)),
              function(i) {
                  land = noisewalker::getLandscape(
                      landsize = params[i]$landsize,
                      octaves = params[i]$octaves,
                      frequency = params[i]$frequency,
                      increment = params[i]$increment,
                      clamp = params[i]$clamp
                  )
                  
                  # autocorrelation structure on a single row
                  # this is very very crude but likely indicative
                  acfstr = get_acf_vals(land[1, ])
                  # add parameters

                  # make dataframe in the form x, y, value
                  land = as.data.table(land)
                  setnames(land, as.character(seq(ncol(land))))
                  land$x = seq(nrow(land)) * params[i]$increment
                  land = melt(land, id.vars = "x", 
                              variable.name = "y")
                  land$y = as.numeric(land$y) * params[i]$increment
                  
                  # return land and autocorrelation range
                  list(land = land, acfstr = acfstr)
              })

# add sample agents
agents = data.table(
    x = runif(500, 0, 10),
    y = runif(500, 0, 10)
)

# clamp at a value
clamp = function(x, clamp_value) {
    x[x < clamp_value] = 0
    x = scales::rescale(x, 0, 1)
    x
}

# combine and plot
subplots = Map(function(df, p, l, i) {

    df$land$value[df$land$value > 0] = 0

    pl = ggplot(df$land)+
        geom_tile(
            aes(x,y,
                fill = value
            )
        )+
        geom_point(
            data = agents,
            aes(x, y),
            col = "red",
            # size = 0.2,
            shape = 1,
            alpha = 0.5
        )+
        scale_fill_continuous_sequential(
            palette = "Heat 2",
            rev = F,
            limits = c(-1, 0),
            name = "resource",
            na.value = "grey90",
            breaks = c(0, -0.5, -1)
        )+
        coord_fixed(
            expand = F
        )+
        theme_test()+
        theme(
            legend.key.width = unit(2, units = "mm"),
            legend.position = "right",
            legend.text = element_text(size = 4)
        )+
        labs(title = glue::glue(
            "frequency = {p}
             landsize = {l}
             increment = {i}"
        ))
    
    ggsave(pl, 
        file = sprintf("figures/fig_land_cost_%.2f.png", p),
        width = 6.5,
        height = 6
    )

}, data, params$frequency, params$landsize, params$increment)

# # make acf plots
# subplots_acf = Map(function(df, p, l, i) {
#     ggplot(df$acfstr)+
#         geom_hline(
#             yintercept = c(0, 0.1),
#             lty = c(1, 2)
#         )+
#         geom_line(
#             aes(acflag, acfval),
#             col = "red"
#         )+
#         coord_cartesian(
#             ylim = c(-0.5, 1)
#         )+
#         theme_test()+
#         labs(
#             x = "distance",
#             y = "autocorrelation",
#             title = glue::glue(
#             "frequency = {p}
#              landsize = {l}
#              increment = {i}"
#         ))
# }, data, params$frequency, params$landsize, params$increment)

# wrap plots
# plot_land = 
wrap_plots(subplots, ncol = length(subplots)) +
    plot_layout(
        guides = "collect"
    ) &
    theme(
        legend.position = "right",
        title = element_text(size = 6)
    )
plot_acf = wrap_plots(subplots_acf, ncol = length(subplots)) &
    theme(
        legend.position = "bottom",
        title = element_text(size = 6)
    )

# wrap all plots
plots = wrap_plots(
    plot_land,
    plot_acf,
    design = "A\nA\nA\nB",
    ncol = 1
)

# save
ggsave(
    plots, filename = "figures/fig_landscape_options.png"
)

