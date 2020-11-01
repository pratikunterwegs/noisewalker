
make_jobs <- function(octaves,
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
    Map(function(genmax, tmax, octaves, f, ft, s_range, this_rep) {
      
      if (!dir.exists("jobs")) {
        dir.create("jobs")
      }
      
      # read basic settings
      shebang[2] <- glue::glue('#SBATCH --job-name=run_noisewalker\\
                               _f_{f}_ft{ft}_rep{rep_n}')
      
      # make command and write to file
      command <- glue::glue('Rscript --slave -e "noisewalker::run_noisewalker(\\
        {genmax},{tmax},{octaves},{f},{ft},{s_range}, {this_rep})"')
      
      jobfile <- glue::glue('job_noisewalker_{f}_{ft}_{s_range}_{this_rep}')
      
      writeLines(
        c(shebang, command),
        con = glue::glue("jobs/{jobfile}")
      )
      
      # upload to server
      ssh::scp_upload(s, glue::glue('jobs/{jobfile}'), to = "noisewalker/jobs/")
      
      # schedule job
      ssh::ssh_exec_wait(s, command = c("cd noisewalker/jobs",
                                        glue::glue('sbatch {jobfile}')))
      
    })
  )
  ssh::ssh_disconnect(s)
}