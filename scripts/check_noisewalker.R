# check the simulation
library(devtools)
library(ggplot2)
library(data.table)

Rcpp::compileAttributes()
build()
sink("install_output.log"); install(); sink()

library(noisewalker)
a = noisewalker::run_noisewalker(
    popsize = 20, 
    genmax = 10, 
    timesteps = 10, 
    perception = 1.0, 
    nOctaves = 2, 
    frequency = 2
)

# WORK IN PROGRESS --- UNRELIABLE FROM HERE #

ggplot(b)+
    geom_boxplot(
        aes(factor(strategy),
            moved)
    )

ggplot(d)+
    geom_line(
        aes(gen, prop,
            colour = factor(strategy))
    )+
    scale_x_log10()

params = CJ(t_increment = c(0.1, 0.5, 1),
            frequency = c(1, 2, 3),
            replicate = seq(3))

data = Map(function(ti, fr) {
    noisewalker::run_noisewalker(
        popsize = 500,
        genmax = 1000,
        timesteps = 1000,
        t_increment = ti,
        nOctaves = 2,
        frequency = fr,
        percep_range = 0.1,
        burnin = 200)  
}, params$t_increment, params$frequency)

save(data, file = "data/data_sim_27_04_2021.Rdata")

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

# get data on movement
data_move = lapply(data, function(l) {
    (l[["movement"]])
})
data_copy = copy(params)
data_copy$move = data_move

# unlist
data_copy = data_copy[,unlist(move, recursive = F),
    by = c("t_increment", "frequency", "replicate")]

ggplot(data_copy)+
    geom_boxplot(
        aes(factor(strategy), moved)
    )+
    facet_grid(t_increment ~ frequency)#+
    scale_y_sqrt()
