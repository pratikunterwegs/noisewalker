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
    costCompete = params$costCompete[row_n],
    nOctaves = params$nOctaves[row_n],   
    frequency = params$frequency[row_n],  
    landsize = params$landsize[row_n],   
    clamp = params$clamp[row_n]
)

# add generation to dataframe
data_evolved_pop = Map(function(df, g) {
    df$gen = g
    df
}, data_evolved_pop$pop_data, data_evolved_pop$gens)
data_evolved_pop = data.table::rbindlist(data_evolved_pop)

# get params as named vector
these_params = unlist(params[row_n,])

# append list of params
data_evolved_pop = append(
  data_evolved_pop,
  these_params
)

# name of rdata file
output_file = Reduce(
  paste,
  mapply(
    function(p, np) {
      paste(np, p, sep = "_")
    }, these_params, names(these_params)
  )
)
output_file = glue::glue(
  'data/output/{output_file}.Rdata'
)

# save
save(
  data_evolved_pop,
  file = output_file
)
