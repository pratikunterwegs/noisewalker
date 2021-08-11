# check the simulation
# libraries for data and plots
library(devtools)
library(ggplot2)
library(data.table)

# libraries to build and install
Rcpp::compileAttributes()
Sys.setenv("LIB_GSL" = "C:/local323")
build()
sink("install_output.log"); install(upgrade = "never"); sink()
document()

# load the lib, better to restart R
detach(package:noisewalker)
library(noisewalker)

noisewalker::agent_response(
    -0.2, 0.02, plot = T, stepsize = 2
)

# test run
a = noisewalker::run_noisewalker(
    popsize = 500, 
    genmax = 1000, 
    timesteps = 25, 
    perception = 0.1,
    directions = 4,
    costMove = 0.01,
    freqRes = 1,
    landsize = 2.5,
    clamp = 0.0,
    random_traits = F,
    allow_compete = T,
    scenario = 1,
    pTransmit = 0.25,
    costInfection = 0.02,
    recordPos = F
)

# get data
data = handle_rcpp_out(a[["gendata"]])

# get max grp size preference
# data[, resp := Map(agent_response, coef_nbrs, coef_risk)]
# data[, grp_size_pref := (vapply(resp, which.max, FUN.VALUE = 1L) * 2) - 2]
# data[, resp := NULL]
# 
# # count grp size preference per gen
# data_grp_size = data[, .N, by = c("gen", "grp_size_pref")]
# 
# # plot
# ggplot(data_grp_size)+
#     geom_tile(
#         aes(gen, grp_size_pref, fill = N)
#     )+
#     colorspace::scale_fill_continuous_sequential(
#         palette = "Reds"
#     )

#### some ####
# get last gen pop
data_last = data[gen %in% c(max(gen)),]
data_last[, resp := Map(agent_response, coef_nbrs, coef_risk)]
data_last[, id := seq(nrow(data_last))]
data_last[, grp_size_pref := (vapply(resp, which.max, FUN.VALUE = 1L) * 2) - 2]

data_summary = data_last[, list(
    resp = unlist(resp, recursive = F),
    grpsize = seq(from = 0, to = 20, by = 2)
), by = c("id", "gen")]

ggplot(data_summary)+
    geom_path(
        aes(grpsize, resp, 
            group = interaction(id, gen)
        ),
        alpha = 0.2,
        colour = "steelblue"
    )+
    geom_hline(
        yintercept = 0
    )+
    scale_y_continuous(
        trans = ggallin::pseudolog10_trans,
        limits = c(-2, 2)
    )+
    scale_x_continuous(
        trans = ggallin::ssqrt_trans,
        limits = c(0, 20)
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
