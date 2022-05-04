# check the simulation
# libraries for data and plots
library(devtools)
library(ggplot2)
library(data.table)

# libraries to build and install
Rcpp::compileAttributes()
Sys.setenv("LIB_GSL" = "C:/local323")
{
    sink("install_output.log"); devtools::install(upgrade = "never"); sink()
}
document()

# load the lib, better to restart R
detach(package:noisewalker)
library(noisewalker)

noisewalker::agent_response(
    -0.2, 0.01, plot = T, stepsize = 2
)

params = CJ(
    ptransmit = c(0, 1),
    costInfection = c(seq(0, 1, 0.2)),
    costPredAvoid = c(seq(0.1, 1, 0.2), 1)
)
params[ptransmit == 0, costInfection := 0]
params = unique(params)

# test run
data = Map(params$ptransmit, params$costInfection, params$costPredAvoid, 
           f = function(ptransmit, ci, cpred) {
               d_ = noisewalker::run_noisewalker(
                   popsize = 200, 
                   genmax = 1000, 
                   timesteps = 25, 
                   perception = 0.1,
                   directions = 4,
                   costMove = 0.01,
                   freqRes = 1.0,
                   landsize = 5,
                   clamp = 0.0,
                   random_traits = F,
                   scenario = 1,
                   pTransmit = ptransmit,
                   costInfection = ci, 
                   costPredAvoid = cpred,
                   recordPos = F
               )
               
               d_ = handle_rcpp_out(d_[["gendata"]])
               d_
           })

# get last gen and compare
data_summary = lapply(data, function(df) {
    df = df[gen >= max(gen) - 200]
    df[, resp := Map(coef_nbrs, coef_risk, f = agent_response)]
    df[, id := seq(length(coef_food)), by = "gen"]
    df = df[, list(
        resp = unlist(resp, recursive = F),
        grpsize = seq(from = 0, to = 20, by = 2)
    ), by = c("id", "gen", "coef_risk", "coef_nbrs")]
    df[, resp := tanh(resp), by = "gen"]
    df
})

# number sim
params$sim_id = seq(nrow(params))
data_summary = Map(data_summary, seq(length(data_summary)), 
                   f = function(df, index) {
                       df$sim_id = index
                       df
                   })

# join data summary
data_summary = rbindlist(data_summary)
data_summary = merge(data_summary, params, by = "sim_id")

data_summary = split(data_summary, by = "ptransmit")

# plot
plots = Map(data_summary, f = function(df) {
    ggplot(df)+
        geom_path(
            aes(grpsize, resp, 
                group = interaction(id, gen)
                # col = (-coef_nbrs / (2*coef_risk)) > 1
            ),
            show.legend = F,
            alpha = 0.1
        )+
        geom_hline(
            yintercept = 0,
            col = 2
        )+
        scale_x_continuous(
            # trans = ggallin::ssqrt_trans,
        )+
        facet_grid(
            costInfection ~ costPredAvoid,
            labeller = label_both
        )+
        coord_cartesian(
            expand = T, 
            # ylim = c(-0.05, 0.05),
            xlim = c(0, 10)
        )+
        theme_test()+
        # theme(
        #     strip.background = element_rect(
        #         fill = NA,
        #         colour = NA
        #     ),
        #     axis.text.y = element_blank()
        # )+
        labs(
            x = "Group size",
            y = "Preference",
            title = glue::glue(
                "P transmit = {unique(df$ptransmit)}"
            )
        )
})

# wrap plots
library(patchwork)
wrap_plots(plots)

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
