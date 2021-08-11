# check the simulation
# libraries for data and plots
library(devtools)
library(ggplot2)
library(data.table)

# libraries to build and install
Rcpp::compileAttributes()
Sys.setenv("LIB_GSL" = "C:/local323")
build()
sink("install_output.log"); install(quick = T, upgrade = "never"); sink()
document()

# load the lib, better to restart R
detach(package:noisewalker)
library(noisewalker)

# test run
a = noisewalker::run_noisewalker(
    popsize = 500, 
    genmax = 100, 
    timesteps = 25, 
    perception = 0.1,
    directions = 4,
    costMove = 0.01,
    freqRes = 1,
    landsize = 10,
    clamp = 0.0,
    random_traits = T,
    allow_compete = T,
    scenario = 1,
    pTransmit = 0.5,
    costInfection = 0.01,
    recordPos = F
)

# get data
data = handle_rcpp_out(a[["gendata"]])

# energy plot
ggplot(data)+
    geom_bin2d(
        aes(gen, energy),
        binwidth = c(1, 0.5)
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
        # xlim = c(0, 100)
    )

# get last gen pop
data_last = data[gen %in% c(max(gen), min(gen)),]
data_last[, resp := Map(agent_response, coef_nbrs, coef_risk)]
data_last[, id := rep(seq(nrow(data_last) / 2), 2)]

data_summary = data_last[, list(
    resp = unlist(resp, recursive = F),
    grpsize = seq(from = 0, to = 20, by = 5)
), by = c("id", "gen")]

ggplot(data_summary)+
    geom_path(
        aes(grpsize, resp, 
            group = interaction(id, gen), 
            colour = factor(gen)),
        alpha = 0.2
    )+
    coord_cartesian(expand = F)

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
        option = "A",
        # begin = 0, end = 0.95,
        direction = -1
    )+
    # coord_cartesian(xlim = c(0, 500))+
    facet_wrap(~variable, scales = "free_y")
