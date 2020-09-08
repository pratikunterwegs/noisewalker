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
           frequency = params$frequency)]
}, data, lookup, SIMPLIFY = FALSE)

# rbind list
data_plot <- rbindlist(data)

# plot
ggplot(data_plot)+
  geom_histogram(aes(mass), binwidth = 0.01)+
  facet_grid(octaves ~ frequency, labeller = label_both)
