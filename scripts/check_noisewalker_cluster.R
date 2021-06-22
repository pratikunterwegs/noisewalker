
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
    popsize = 1000, 
    genmax = 1000, 
    timesteps = 100, 
    perception = 0.1,
    directions = 8,
    costMove = c(0.01, 0.1, 0.25),
    costSensing = c(0.01, 0.1, 0.25),
    costCompete = c(0.01, 0.1, 0.25),
    nOctaves = 1, 
    frequency = c(1, 2),
    landsize = 5,
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
