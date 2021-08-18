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
    -0.2, 0.01, plot = T, stepsize = 2
)

# test run
a = noisewalker::run_noisewalker(
    popsize = 500, 
    genmax = 10000, 
    timesteps = 25, 
    perception = 1,
    directions = 4,
    costMove = 0.01,
    freqRes = 2.0,
    landsize = 50,
    clamp = 0.0,
    random_traits = T,
    scenario = 0,
    pTransmit = 0.25,
    costInfection = 0.1, 
    costPredAvoid = 10,
    recordPos = F
)

# get data
data = handle_rcpp_out(a[["gendata"]])

# classify based on coef risk
data[, d2y := coef_risk > 0]
data[, dy := coef_nbrs > 0]

# check dev over time
data_d2y = data[, .N, by = c("d2y", "gen")]
data_dy = data[, .N, by = c("dy", "gen")]

ggplot(data_d2y)+
    geom_path(
        aes(gen, N, col = d2y)
    )
ggplot(data_dy)+
    geom_path(
        aes(gen, N, col = dy)
    )

#### some ####
# get last gen pop
data_last = data[gen %in% floor(seq(0, 1000, length.out = 10)),]
data_last[, resp := Map(agent_response, coef_nbrs, coef_risk)]
data_last[, id := rep(seq(500), 10)]

data_summary = data_last[, list(
    resp = unlist(resp, recursive = F),
    grpsize = seq(from = 0, to = 20, by = 2)
), by = c("id", "gen", "coef_risk", "coef_nbrs")]

ggplot(data_summary)+
    geom_hline(
        yintercept = 0
    )+
    geom_path(
        aes(grpsize, resp, 
            group = interaction(id, gen)
            # col = (-coef_nbrs / (2*coef_risk)) > 1
        ),
        col = "steelblue",
        alpha = 0.1
    )+
    scale_y_continuous(
        trans = ggallin::pseudolog10_trans
    )+
    scale_x_continuous(
        # trans = ggallin::ssqrt_trans,
        # limits = c(0, 10)
    )+
    facet_wrap(~gen, scales = "free_y")+
    coord_cartesian(expand = F, 
                    # ylim = c(-0.05, 0.05),
                    xlim = c(0, 20))

#### weight evolution ####
# tanh transform
data[, c("coef_food", "coef_nbrs", "coef_risk") := lapply(
    .SD, function(x) {
        cut_wt_lower(x, steps = 100, scale = 40)
    }
), .SDcols = c("coef_food", "coef_nbrs", "coef_risk")]

# melt data
data[, c("d2y", "dy", "inflection") := NULL]
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
        option = "F",
        # begin = 0, end = 0.95,
        direction = -1
    )+
    # coord_cartesian(xlim = c(0, 500))+
    facet_wrap(~variable, scales = "free_y")+
    theme(
        legend.key.width = unit(2, "mm")
    )
