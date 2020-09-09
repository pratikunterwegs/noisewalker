#### analyse pop mass distribution ####
library(data.table)
library(ggplot2)

files <- list.files("data", full.names = TRUE, 
                    pattern = "(\\d+)\\.(csv)")
lookup <- fread("data/lookup.csv")

# split lookup
lookup <- split(lookup, lookup$filename)

# read in
data <- lapply(files, fread)

# link to file
data <- mapply(function(df, params) {
  df[,`:=`(octaves = params$octaves,
           frequency = params$frequency,
           repl = params$rep)]
}, data, lookup, SIMPLIFY = FALSE)

# rbind list
data_plot <- rbindlist(data)

# plot
ggplot(data_plot)+
  stat_density(aes(mass, col = factor(repl)), 
                 position = "identity", alpha = 1,
               geom = "line", lwd = 0.5)+
  scale_colour_manual(values = pals::kovesi.rainbow(5))+
  facet_grid(frequency ~ octaves, labeller = label_both,
             scales = "free")+
  theme_bw()+
  coord_cartesian(xlim = c(0, 3))

ggsave(filename = "figures/fig_mass_histogram.png", dpi = 300)
