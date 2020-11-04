
#' Run noisewalker on Peregrine.
#'
#' @param octaves The integer number of octaves of the landscape, representing
#' small scale variability. This function is \emph{NOT WORKING}.
#' Set to a default value of 1. It is unclear what the \code{fastnoise}
#' default actually is.
#' @param f Frequency of the ancestral landscape.
#' @param ft Frequency of the transplant landscape.
#' @param s_range Individual sensory range.
#' @param genmax Generations on each landscape.
#' @param tmax Timesteps per generation.
#' @param n_rep How many replicates to run.
#' @param ssh_con The remote machine to connect to.
#' This should be of the form 'p123456@peregrine.hpc.rug.nl'
#' @param password The SSH connection password.
#'
#' @return Runs the noisewalker simulation on Peregrine or another cluster.
#' @export
#'
make_jobs <- function(octaves = 1L,
                      f, ft, s_range,
                      genmax = 10000,
                      tmax = 100,
                      n_rep = 3,
                      ssh_con = "some server",
                      password = "your password") {

  # make crossing
  params <- data.table::CJ(
    octaves,
    f, ft, s_range,
    genmax, tmax,
    run = as.character(seq_len(n_rep))
  )

  # connect to server
  s <- ssh::ssh_connect(ssh_con, passwd = password)

  # read job shebang
  shebang <- readLines("template_job.sh")
  invisible(
    Map(function(genmax, tmax, octaves, fnow, ft, s_range, this_rep) {
      if (!dir.exists("jobs")) {
        dir.create("jobs")
      }

      # read basic settings
      shebang[2] <- glue::glue("#SBATCH --job-name=run_noisewalker\\
                               _f_{fnow}_ft{ft}_rep{this_rep}")

      # make command and write to file
      # command_build <- "Rscript -e 'devtools::install()'"
      command_main <- glue::glue('Rscript --slave -e \\
        "noisewalker::run_noisewalker(\\
        {genmax},{tmax},{octaves},{fnow},{ft},{s_range}, \'{this_rep}\')"')

      jobfile <- glue::glue("job_noisewalker_{fnow}_{ft}_{s_range}_{this_rep}")

      writeLines(
        c(
          shebang,
          # command_build,
          command_main
        ),
        con = glue::glue("jobs/{jobfile}")
      )

      # upload to server
      ssh::scp_upload(s, glue::glue("jobs/{jobfile}"), to = "noisewalker")

      # schedule job
      ssh::ssh_exec_wait(s, command = c(
        "cd noisewalker",
        glue::glue("sbatch {jobfile}")
      ))
    },
    genmax = params$genmax, tmax = params$tmax,
    octaves = params$octaves,
    fnow = params$f, ft = params$ft,
    s_range = params$s_range,
    this_rep = params$run
    )
  )
  # remove all job files
  ssh::ssh_exec_wait(s, command = "rm noisewalker/job_noisewalker*")
  ssh::ssh_disconnect(s)
}
