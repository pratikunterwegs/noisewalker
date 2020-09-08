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
  df[,`:=`(frequency = params$frequency,
           tempRate = params$tempRate)]
}, data, lookup, SIMPLIFY = FALSE)

# rbind list
data_plot <- rbindlist(data)

x11()
# plot
ggplot(data_plot[id %% 1 ==0,])+
  geom_vline(xintercept = 0, col = "red", lty = 2,
             size = 0.1)+
  geom_line(aes(cue, distance + 0.1,
                group = id,
                colour = ifelse(distance < 0.01, "non-move", "move")),
            alpha = 0.1,
            show.legend = F)+
  facet_grid(tempRate ~ frequency, 
             labeller = label_both)+
  theme_void()+
  coord_cartesian(ylim=c(0.1, 10),
                  xlim=c(-0.5, 0.5))
ggsave(filename = "figures/fig_rnorm_08_sep_2020.png", dpi = 300)
