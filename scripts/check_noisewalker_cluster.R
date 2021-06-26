
library(noisewalker)
password = readLines("data/password")

# check cluster
noisewalker::check_prepare_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password
)

# prepare parameters
noisewalker::make_parameter_file(
    replicates = 5,
    popsize = 5000, 
    genmax = 10000, 
    timesteps = 100, 
    perception = 0.05,
    directions = 4,
    costMove = c(0.1, 1),
    costCompete = c(0.1, 1),
    nOctaves = 1, 
    frequency = c(1, 2, 8),
    landsize = 10,
    clamp = 0,
    which_file = "data/parameters/parameters_full.csv"
)

# try sending in a job
noisewalker::use_cluster(
  ssh_con = "p284074@peregrine.hpc.rug.nl",
  password = password, 
  script = "scripts/run_sim_cluster.R", 
  template_job = "bash/main_job_maker.sh", 
  parameter_file = "data/parameters/parameters_full.csv"
)
