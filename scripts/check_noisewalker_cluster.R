
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
    perception = 1,
    directions = 8,
    costMove = 0.01,
    costSensing = 0.01,
    costCompete = 10 ^ seq(-3, 0, 1),
    nOctaves = 2, 
    frequency = c(2, 4, 8),
    landsize = 50,
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
