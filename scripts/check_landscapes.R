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
    landsize = 5,
    octaves = 1,
    frequency = c(0.1, 0.25, 0.5, 1, 2, 4),
    increment = 0.05,
    clamp = 0
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
                  land$x = seq(nrow(land))
                  land = melt(land, id.vars = "x", 
                              variable.name = "y")
                  land$y = as.numeric(land$y)
                  
                  # return land and autocorrelation range
                  list(land = land, acfstr = acfstr)
              })

# combine and plot
subplots = Map(function(df, p, l, i) {
    ggplot(df$land)+
        geom_tile(
            aes(x,y,fill = value)
        )+
        scale_fill_viridis_c(
            option = "B",
            limits = c(0, 1)
        )+
        coord_fixed()+
        theme_void()+
        labs(title = glue::glue(
            "frequency = {p}
             landsize = {l}
             increment = {i}"
        ))
}, data, params$frequency, params$landsize, params$increment)

# make acf plots
subplots_acf = Map(function(df, p, l, i) {
    ggplot(df$acfstr)+
        geom_hline(
            yintercept = c(0, 0.1),
            lty = c(1, 2)
        )+
        geom_line(
            aes(acflag, acfval),
            col = "red"
        )+
        coord_cartesian(
            ylim = c(-0.5, 1)
        )+
        theme_test()+
        labs(title = glue::glue(
            "frequency = {p}
             landsize = {l}
             increment = {i}"
        ))
}, data, params$frequency, params$landsize, params$increment)

# wrap plots
plot_land = wrap_plots(subplots, ncol = length(subplots)) +
    plot_layout(
        guides = "collect"
    ) &
    theme(
        legend.position = "bottom"
    )
plot_acf = wrap_plots(subplots_acf, ncol = length(subplots))

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

