library(data.table)
library(glue)

f <- seq(1.0, 8.0, 1)
t <- 2^(-6:6)

params <- CJ(f, t)

commands <- glue_data(.x = params,
                      './noisewalker 100 100 2 {f} {t} 1.0 1')

# write shell script
writeLines(
  text = c(
    "#! /bin/bash",
    commands
  ),
  con = "run_sim_long.sh"
)
