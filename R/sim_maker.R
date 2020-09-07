library(data.table)
library(glue)

f <- seq(0.5, 8.0, 0.5)
t <- 10^(-3:3)

params <- CJ(f, t)

commands <- glue_data(.x = params,
                      './noisewalker 1000 10 2 {f} {t} 1.0 1')

# write shell script
writeLines(
  text = c(
    "#! /bin/bash",
    commands
  ),
  con = "run_sim.sh"
)
