# check the simulation
library(devtools)
library(ggplot2)
library(data.table)

build()
install()

library(noisewalker)
a = noisewalker::run_noisewalker(
    popsize = 100,
    genmax = 50,
    timesteps = 100,
    t_increment = 1,
    nOctaves = 2,
    frequency = 2,
    landsize = 100
)

params = CJ(t_increment = c(0.1, 1, 10),
            frequency = c(2,4))

data = Map(function(ti, fr, re) {
    noisewalker::run_noisewalker(
        popsize = 50,
        genmax = 10,
        timesteps = 100,
        t_increment = ti,
        nOctaves = 2,
        frequency = fr,
        landsize = 100)  
}, params$t_increment, params$frequency)


b = do.call(rbind.data.frame, a)

ggplot(b)+
    geom_hline(yintercept = 0.33,
               col = "grey")+
    geom_path(
        aes(gen, prop,
            colour = factor(strategy))
    )
