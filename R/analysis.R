library(data.table)
library(ggplot2)

files <- list.files("data", full.names = TRUE, 
                    pattern = "(\\d+)\\.(csv)")
lookup <- fread("data/lookup.csv")

# split lookup
lookup <- split(lookup, lookup$filename)

# read in
data <- lapply(files[4], fread)

# link to file
data <- mapply(function(df, params) {
  df[,`:=`(frequency = params$frequency,
           tempRate = params$tempRate)]
}, data, lookup, SIMPLIFY = FALSE)

# rbind list
data <- rbindlist(data)

# plot
ggplot(data[1:1e4,])+
  geom_line(aes(cue, distance,
                group = id), alpha = 0.1)+
  facet_grid(tempRate ~ frequency, 
             labeller = label_both)
