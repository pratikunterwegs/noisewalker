#!/bin/bash
#SBATCH --time=03:00:00
#SBATCH --partition=regular
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --job-name=noisewalker_simulation
#SBATCH --array=1-n_array
#SBATCH --output=/home/p284074/noisewalker/data/output/noisewalker_output_%a.log
#SBATCH --mem=5000

pwd
ml load R/4.0.0-foss-2020a
cd noisewalker
echo "now in" 
pwd
Rscript some rscript here ${SLURM_ARRAY_TASK_ID}
