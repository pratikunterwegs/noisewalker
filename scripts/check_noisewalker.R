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

params = CJ(t_increment = c(0.5, 1, 2),
            frequency = c(1, 4, 8),
            replicate = seq(5))

data = Map(function(ti, fr) {
    noisewalker::run_noisewalker(
        popsize = 500,
        genmax = 1000,
        timesteps = 50,
        t_increment = ti,
        nOctaves = 2,
        frequency = fr,
        landsize = 100)  
}, params$t_increment, params$frequency)

save(data, file = "data/data_sim_19_04_2021.Rdata")

data_fitness = lapply(data, function(l) {
    rbindlist(l[["fitness"]])
})

data_copy = copy(params)
data_copy$fitness = data_fitness

# unlist
data_copy = data_copy[,unlist(fitness, recursive = F),
    by = c("t_increment", "frequency", "replicate")]

ggplot(data_copy[gen %% 10 == 0])+
    geom_hline(yintercept = 0.33,
               col = "grey")+
    geom_point(
        aes(gen, mean_fitness,
            # colour = factor(strategy),
            group = interaction(replicate)),
        shape = 1
    )+
    ylim(0, NA)+
    facet_grid(t_increment ~ frequency)

# see strategy 
data_strat = lapply(data, function(l) {
    rbindlist(l[["pop_comp"]])
})
data_copy = copy(params)
data_copy$data = data_strat
data_copy = data_copy[, unlist(data, recursive = F),
    by = c("t_increment", "frequency", "replicate")]

ggplot(data_copy)+
    geom_hline(yintercept = 0.33,
               col = "grey")+
    geom_path(
        aes(gen, prop,
            colour = factor(strategy),
            group = interaction(strategy, replicate))
    )+
    facet_grid(t_increment ~ frequency)+
    scale_x_sqrt()

# see distance moved
ggplot(data_copy[!is.infinite(mean_distance)])+
    geom_hline(yintercept = 0.33,
               col = "grey")+
    geom_point(
        aes(gen, mean_distance,
            colour = factor(strategy),
            group = interaction(strategy, replicate)),
        shape = 1
    )+
    facet_grid(t_increment ~ frequency)+
    scale_x_sqrt()+
    scale_y_log10(lim = c(0.1, 1000))
