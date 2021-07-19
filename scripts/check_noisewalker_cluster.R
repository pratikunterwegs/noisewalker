
library(noisewalker)
password = readLines("data/password")

# check cluster
# noisewalker::check_prepare_cluster(
#   ssh_con = "p284074@peregrine.hpc.rug.nl",
#   password = password
# )

# prepare parameters
noisewalker::make_parameter_file(
    replicates = 10,
    popsize = 1000, 
    genmax = 1000, 
    timesteps = 100, 
    perception = 0.1,
    directions = 4,
    costMove = 0.001,
    freqRes = c(0.1, 0.5, 1),
    landsize = 10,
    clamp = 0.0,
    random_traits = F,
    allow_compete = c(T),
    scenario = c(0, 1, 2),
    pTransmit = c(0.1, 0.5, 1),
    costInfection = 0.01,
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
