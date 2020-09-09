library(data.table)
library(glue)

f <- seq(1.0, 8.0, 1)
octaves <- seq_len(8)
repl <- seq(5)

params <- CJ(f, octaves, repl)

# of the form noisewalker genmax timesteps octaves freq temprate srange rep
commands <- glue_data(.x = params,
                      './noisewalker 1000 100 {octaves} {f} 1.0 1.0 {repl}')

# write shell script
writeLines(
  text = c(
    "#! /bin/bash",
    commands
  ),
  con = "run_sim_mass.sh"
)
