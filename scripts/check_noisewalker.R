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
detach(package:noisewalker)
library(noisewalker)

# test run
a = noisewalker::run_noisewalker(
    popsize = 100, 
    genmax = 50, 
    timesteps = 25, 
    perception = 0.1,
    directions = 4,
    costMove = 0.0,
    freqRes = 1,
    landsize = 5,
    clamp = 0.0,
    random_traits = F,
    allow_compete = T,
    scenario = 1,
    pTransmit = 0.5,
    costInfection = 0.01
)

data = a[["posdata"]]

posdata = Map(function(df, g) {
    df$gen = g
    df
}, data[["pos_data"]], data[["gens"]])

# bind
posdata = rbindlist(posdata)

# plot
ggplot(posdata[gen %% 10 == 0,])+
    geom_bin2d(
        aes(x, y)
    )+
    scale_fill_viridis_c(
        option = "F", direction = -1,
        trans = "log10"
    )+
    facet_wrap(~gen)+
    coord_fixed()

# save data
save(a, file = "data/output/test_data.Rds")

# get data
data = handle_rcpp_out(a[["gendata"]])

# energy plot
ggplot(data)+
    geom_bin2d(
        aes(gen, energy),
        binwidth = c(1, 0.1)
    )+
    # geom_point(
    #     aes(gen, energy),
    #     alpha = 0.1,
    #     shape = 1
    # )+
    scale_fill_viridis_c(
        # trans = "log10",
        direction = -1
    )+
    coord_cartesian(
        # ylim = c(0, 10)
    )


# tanh transform
data[, c("coef_food", "coef_nbrs", "coef_risk") := lapply(
    .SD, function(x) {
        cut_wt_lower(x, steps = 100, scale = 20)
    }
), .SDcols = c("coef_food", "coef_nbrs", "coef_risk")]

# melt data
data = melt(data[,!c("energy","moved","time_infected","interactions")], id.vars = "gen")

# count by weight value
data_summary = data[, list(.N), 
                    by = c("gen", "variable", "value")]

# bin 2d
ggplot(data_summary)+
    geom_tile(
        aes(gen, value, fill = N)
    )+
    scale_fill_viridis_c(
        option = "C",
        begin = 0, end = 0.95,
        direction = -1
    )+
    # coord_cartesian(xlim = c(0, 500))+
    facet_wrap(~variable, scales = "free_y")
