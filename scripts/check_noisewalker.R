# check the simulation
library(devtools)
library(ggplot2)
library(data.table)

build()
install()

library(noisewalker)
a = noisewalker::run_noisewalker(
    popsize = 100,
    genmax = 60,
    timesteps = 100,
    t_increment = 1,
    nOctaves = 2,
    frequency = 2,
    landsize = 200
)

params = CJ(t_increment = c(0.01, 0.1, 1, 10),
            frequency = c(2,4),
            replicate = seq(5))

data = Map(function(ti, fr) {
    noisewalker::run_noisewalker(
        popsize = 1000,
        genmax = 500,
        timesteps = 100,
        t_increment = ti,
        nOctaves = 2,
        frequency = fr,
        landsize = 100)  
}, params$t_increment, params$frequency)

data = lapply(data, rbindlist)

data_copy = copy(params)
data_copy$data = data

# unlist
data_copy = data_copy[,unlist(data, recursive = F),
    by = c("t_increment", "frequency", "replicate")]

ggplot(data_copy)+
    geom_hline(yintercept = 0.33,
               col = "grey")+
    geom_path(
        aes(gen, prop,
            colour = factor(strategy),
            group = interaction(strategy, replicate))
    )+
    facet_grid(t_increment ~ frequency)
