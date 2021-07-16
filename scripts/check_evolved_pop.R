# code to check evolved populations
library(data.table)
library(ggplot2)
library(patchwork)

# list output
output = list.files(path = "data/output/",
                    pattern = "output_file",
                    full.names = T)

# check single trait
# load files
lapply(output, function(file) {
  message(
    glue::glue(
      "reading {file}"
    )
  )
  load(file)
  df = data_evolved_pop[[1]]
  data.table::setDT(df)
  
  params = data_evolved_pop[[2]]
  
  # plot and check
  p =
    ggplot(df)+
    geom_bin2d(
      aes(gen, energy), 
      binwidth = c(2, 0.5),
      show.legend = F
    )+
    scale_fill_viridis_c(
      option = "C", 
      direction = -1
    )+
    coord_cartesian(
      expand = T
      # xlim = c(0, 1)
    )+
    theme(
      # axis.text = element_blank(),
      title = element_text(size = 8)
    )+
    # facet_wrap(~gen, nrow = 1)+
    labs(
      title = glue::glue(
        "frq = {params['freqRes']} \\
        sc = {params['scenario']} \\
        compete = {params['allow_compete']} \\
        rep = {params['replicate']}",
      ),
      x = "generation", y = "energy"
    )
  
  # process figures for weight evol
  # tanh transform
  df[, c("coef_food", "coef_nbrs", "coef_risk") := lapply(
    .SD, function(x) {
      cut_wt_lower(x, steps = 100, scale = 40)
    }
  ), .SDcols = c("coef_food", "coef_nbrs", "coef_risk")]
  
  # melt data
  df = data.table::melt(df[,!c("energy","moved","time_infected")], 
                        id.vars = "gen")
  
  # count by weight value
  data_summary = df[, list(N = .N / params['popsize']), 
                    by = c("gen", "variable", "value")]
  
  # bin 2d
  p2 = ggplot(data_summary[N > 0.01,])+
    geom_tile(
      aes(gen, value, fill = N)
    )+
    scale_fill_viridis_c(
      option = "C",
      begin = 0, end = 0.95,
      direction = -1
    )+
    # coord_cartesian(xlim = c(0, 500))+
    facet_wrap(~variable, scales = "free_y")+
    theme(
      # axis.text = element_blank(),
      title = element_text(size = 8),
      legend.key.width = unit(2, units = "mm")
    )+
    labs(
      title = glue::glue(
        "frq = {params['freqRes']} \\
        sc = {params['scenario']} \\
        compete = {params['allow_compete']} \\
        rep = {params['replicate']}",
      )
    )
  
  fig = wrap_plots(p, p2, design = "ABBB")
  filename = glue::glue("figures/fig_frq_{params['freqRes']}\\
      _sc_{params['scenario']}_comp_{params['allow_compete']}\\
      _rep_{params['replicate']}.png")
  
  ggsave(
    fig, height = 3, width = 12,
    filename = filename
  )
})

