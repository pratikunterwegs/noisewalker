args = commandArgs(TRUE)

message(getwd())

param_file = args[1]

row_n = as.numeric(args[2])

message(
  paste("which file = ", param_file)
)

message(
  paste("which row = ", row_n)
)

params = read.csv(param_file)

library(noisewalker)

# run simulation
data_evolved_pop = run_noisewalker(
    popsize = params$popsize[row_n], 
    genmax = params$genmax[row_n],
    timesteps = params$timesteps[row_n],
    perception = params$perception[row_n],    
    directions = params$directions[row_n],  
    costMove = params$costMove[row_n],
    freqRes = params$freqRes[row_n],
    landsize = params$landsize[row_n],   
    clamp = params$clamp[row_n],
    random_traits = params$random_traits[row_n],
    allow_compete = params$allow_compete[row_n],
    scenario = params$scenario[row_n],
    pTransmit = params$pTransmit[row_n],
    costInfection = params$costInfection[row_n]
)

# add generation to dataframe
data_evolved_pop = Map(function(df, g) {
    df$gen = g
    df
}, data_evolved_pop[["gendata"]]$pop_data, data_evolved_pop[["gendata"]]$gens)
data_evolved_pop = data.table::rbindlist(data_evolved_pop)

# add posdata to dataframe
data_final_pos = Map(function(df, g) {
    df$gen = g
    df
}, data_evolved_pop[["posdata"]]$pop_data, data_evolved_pop[["posdata"]]$gens)
data_final_pos = data.table::rbindlist(data_final_pos)

# get params as named vector
these_params = unlist(params[row_n,])

# append list of params
data_evolved_pop = list(
  gendata = data_evolved_pop,
  posdata = data_final_pos,
  these_params
)

row_n = stringr::str_pad(row_n, 4, "left", 0)

# name of rdata file and export
output_file = glue::glue(
  'data/output/output_file_{row_n}.Rds'
)

# save
save(
  data_evolved_pop,
  file = output_file
)
