# check the simulation
# libraries for data and plots
library(devtools)
library(ggplot2)
library(data.table)

# libraries to build and install
Rcpp::compileAttributes()
build()
sink("install_output.log"); install(); sink()
document()

# load the lib, better to restart R
library(noisewalker)
# test run
a = noisewalker::run_noisewalker(
    popsize = 1000, 
    genmax = 500, 
    timesteps = 50, 
    perception = 0.05,
    directions = 4,
    costMove = 0.01,
    costCompete = 1,
    nOctaves = 2, 
    frequency = 2,
    landsize = 5,
    clamp = 0.0
)

# get data
data = Map(function(df, g) {
    df$gen = g
    df
}, a$pop_data, a$gens)
data = rbindlist(data)

# tanh transform
data[, c("coef_food", "coef_nbrs") := lapply(
    .SD, function(x) tanh(x)
), .SDcols = c("coef_food", "coef_nbrs")]

# melt data
data = melt(data[,!"energy"], id.vars = "gen")

# bin 2d
ggplot(data)+
    geom_bin2d(
        aes(gen, value),
        binwidth = c(2, 0.02)
    )+
    scale_fill_viridis_c(
        option = "D",
        direction = -1
    )+
    coord_cartesian(ylim = c(-2,2))+
    facet_grid(~variable)

# do bin 2d
ggplot(data[gen %% 10 == 0, ])+
    geom_bin2d(
        aes(coef_food, coef_nbrs),
        binwidth = c(0.05, 0.05)
    )+
    scale_fill_viridis_c(option = "C", direction = -1)+
    facet_wrap(~gen)+
    coord_fixed(
        xlim = c(0, 1),
        ylim = c(0, 1)
    )+
    theme_test(base_size = 6)+
    theme(
        axis.title = element_text(size = 12)
    )+
    labs(
        x = "coef(Food)", y = "coef(Nbrs)"
    )
ggsave(
    filename = "figures/fig_prelim_plot.png"
)

# summarise
data[,c("actv", "resp") := list(
    plyr::round_any(actv, 0.05), plyr::round_any(resp, 0.05)
)]
data_summary = data[,.N, by = c("gen", "actv", "resp")]

# plot as tile for some generations
# data_summary = data_summary[gen %% 10 == 0,]

ggplot(data_summary)+
    geom_tile(
        aes(actv, resp, fill = N)
    )+
    scale_fill_viridis_c(option = "F", direction = -1,
                         trans = "log10")+
    # coord_fixed(
    #     xlim = c(0, 1),
    #     ylim = c(0, 1)
    # )+
    theme_grey(base_size = 6)+
    facet_wrap(~gen, ncol = 16)

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
