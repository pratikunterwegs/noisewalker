# check the simulation
# libraries for data and plots
library(ggplot2)
library(patchwork)
library(data.table)

# load the lib, better to restart R
library(noisewalker)

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
                  # make dataframe in the form x, y, value
                  land = as.data.table(land)
                  setnames(land, as.character(seq(ncol(land))))
                  land$x = seq(nrow(land))
                  land = melt(land, id.vars = "x", 
                              variable.name = "y")
                  land$y = as.numeric(land$y)
                  
                  # return land
                  land
              })

# combine and plot
subplots = Map(function(df, p, l, i) {
    ggplot(df)+
        geom_tile(
            aes(x,y,fill = value)
        )+
        scale_fill_viridis_c(
            option = "B"
        )+
        coord_fixed()+
        theme_void()+
        labs(title = glue::glue(
            "frequency = {p}
             landsize = {l}
             increment = {i}"
        ))
}, data, params$frequency, params$landsize, params$increment)

# wrap plots
plot = wrap_plots(subplots, ncol = 3)

# save
ggsave(
    plot, filename = "figures/fig_landscape_options.png"
)