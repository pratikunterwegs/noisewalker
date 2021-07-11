# code to check evolved populations
library(data.table)
library(ggplot2)
library(patchwork)

# list output
output = list.files(path = "data/output/",
                    pattern = "frequency_1",
                    full.names = T)

# check single trait
# load files
invisible(
  lapply(output, function(file) {
    load(file)
    df = as.data.table(
      data_evolved_pop[c("actv", "gen")]
    )
    
    # plot and check
    p = ggplot(df)+
      geom_bin2d(
        aes(gen, actv), 
        binwidth = c(5, 0.025),
        show.legend = F
      )+
      scale_fill_viridis_c(
        option = "F", direction = -1,
        trans = "log10",
        begin = 0.1, end = 1
      )+
      coord_cartesian(expand = T,
                      # xlim = c(0, 1),
                      ylim = c(0, 1))+
      theme_grey()+
      theme(
        axis.text = element_blank(),
        title = element_text(size = 8)
      )+
      # facet_wrap(~gen, nrow = 1)+
      labs(
        title = glue::glue(
          "frq = {data_evolved_pop$frequency} \\
        cM = {data_evolved_pop$costMove} \\
        cS = {data_evolved_pop$costMove} \\
        cC = {data_evolved_pop$costCompete} \\
        rep = {data_evolved_pop$replicate}",
        ),
        x = "generation", y = "activity"
      )
    
    ggsave(
      p,
      filename = glue::glue(
        "figures/fig_actv_frq{data_evolved_pop$frequency}_\\
      cM_{data_evolved_pop$costMove}_\\
      cS_{data_evolved_pop$costMove}_\\
      cC_{data_evolved_pop$costCompete}_\\
      rep_{data_evolved_pop$replicate}.png"
      )
    )
    rm(p)
  })
)

# plot responsiveness
invisible(
  lapply(output, function(file) {
    load(file)
    df = as.data.table(
      data_evolved_pop[c("resp", "gen")]
    )
    
    # plot and check
    p = ggplot(df)+
      geom_bin2d(
        aes(gen, resp), 
        binwidth = c(5, 0.025),
        show.legend = F
      )+
      scale_fill_viridis_c(
        option = "G", direction = -1,
        trans = "log10",
        begin = 0.1, end = 1
      )+
      coord_cartesian(expand = T,
                      # xlim = c(0, 1),
                      ylim = c(0, 1))+
      theme_grey()+
      theme(
        axis.text = element_blank(),
        title = element_text(size = 8)
      )+
      # facet_wrap(~gen, nrow = 1)+
      labs(
        title = glue::glue(
          "frq = {data_evolved_pop$frequency} \\
        cM = {data_evolved_pop$costMove} \\
        cS = {data_evolved_pop$costSensing} \\
        cC = {data_evolved_pop$costCompete}",
        ),
        x = "generation", y = "response"
      )
    ggsave(
      p,
      filename = glue::glue(
        "figures/fig_resp_frq{data_evolved_pop$frequency}_\\
      cM_{data_evolved_pop$costMove}_\\
      cS_{data_evolved_pop$costMove}_\\
      cC_{data_evolved_pop$costCompete}_\\
      rep_{data_evolved_pop$replicate}.png"
      )
    )
    rm(p)
  })
)

# wrap plots
wrap_plots(plots_resp, ncol = 5)


# load files
invisible(
lapply(output, function(file) {
  load(file)
  df = as.data.table(
    data_evolved_pop[c("actv", "resp", "gen")]
  )
  
  # plot and check
  p = ggplot(df[gen %in% c(0, 100, 500, max(gen))])+
    geom_bin2d(
      aes(actv, resp),
      col = "grey20",
      size = 0.1,
      binwidth = c(0.05, 0.05),
      show.legend = F
    )+
    geom_hline(
      yintercept = 0.5,
      size = 0.2
    )+
    geom_vline(
      xintercept = 0.5,
      size = 0.2
    )+
    scale_fill_viridis_c(
      option = "D", direction = -1
    )+
    coord_cartesian(expand = T,
                xlim = c(0, 1),
                ylim = c(0, 1))+
    theme_grey()+
    theme(
      axis.text = element_blank(),
      title = element_text(size = 8)
    )+
    facet_wrap(~gen, nrow = 1)+
    labs(
      title = glue::glue(
        "frq = {data_evolved_pop$frequency} \\
        cM = {data_evolved_pop$costMove} \\
        cS = {data_evolved_pop$costSensing} \\
        cC = {data_evolved_pop$costCompete}",
      )
    )
  ggsave(
    p,
    filename = glue::glue(
      "figures/fig_2d_frq{data_evolved_pop$frequency}_\\
      cM_{data_evolved_pop$costMove}_\\
      cS_{data_evolved_pop$costMove}_\\
      cC_{data_evolved_pop$costCompete}_\\
      rep_{data_evolved_pop$replicate}.png"
    ),
    height = 2,
    width = 6
  )
  rm(p)
})
)

# wrap plots
wrap_plots(
  plots, ncol = 5
)
