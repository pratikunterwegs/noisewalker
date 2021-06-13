# code to check evolved populations
library(data.table)
library(ggplot2)
library(patchwork)

# list output
output = list.files(path = "data/output/",
                    pattern = "costMove_0.1", full.names = T)

# check single trait
# load files
plots_act = lapply(output, function(file) {
  load(file)
  df = as.data.table(
    data_evolved_pop[c("actv", "gen")]
  )
  
  # plot and check
  ggplot(df)+
    geom_bin2d(
      aes(gen, actv), 
      binwidth = c(3, 0.025),
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
      axis.text = element_blank()
    )+
    # facet_wrap(~gen, nrow = 1)+
    labs(
      title = glue::glue(
        "frequency = {data_evolved_pop$frequency}",
      ),
      x = "generation", y = "activity"
    )
  })

# wrap plots
wrap_plots(plots_act, ncol = 5)

# plot responsiveness
plots_resp = lapply(output, function(file) {
  load(file)
  df = as.data.table(
    data_evolved_pop[c("resp", "gen")]
  )
  
  # plot and check
  ggplot(df)+
    geom_bin2d(
      aes(gen, resp), 
      binwidth = c(3, 0.025),
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
      axis.text = element_blank()
    )+
    # facet_wrap(~gen, nrow = 1)+
    labs(
      title = glue::glue(
        "frequency = {data_evolved_pop$frequency}",
      ),
      x = "generation", y = "response"
    )
})

# wrap plots
wrap_plots(plots_resp, ncol = 5)


# load files
plots = lapply(output, function(file) {
  load(file)
  df = as.data.table(
    data_evolved_pop[c("actv", "resp", "gen")]
  )
  
  # plot and check
  ggplot(df[gen %in% c(50, 100, 500, max(gen))])+
    geom_bin2d(
      aes(actv, resp), 
      binwidth = c(0.05, 0.05),
      show.legend = F
    )+
    scale_fill_viridis_c(
      option = "C", direction = -1,
      trans = "log10"
    )+
    coord_fixed(expand = T,
                xlim = c(0, 1),
                ylim = c(0, 1))+
    theme_grey()+
    theme(
      axis.text = element_blank()
    )+
    facet_wrap(~gen, nrow = 1)+
    labs(
      title = glue::glue(
        "frequency = {data_evolved_pop$frequency}"
      )
    )
})

# wrap plots
wrap_plots(
  plots, ncol = 5
)
