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
data_plot <- rbindlist(data[4])

# plot
ggplot(data_plot)+
  geom_vline(xintercept = 0, col = "red", lty = 2)+
  geom_line(aes(cue, distance + 0.1,
                group = id), 
            colour = "grey20",
            alpha = 0.1)+
  facet_grid(tempRate ~ frequency, 
             labeller = label_both)
