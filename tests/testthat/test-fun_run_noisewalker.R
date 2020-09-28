context("noisewalker works")
testthat::test_that("noisewalker works", {

  testthat::expect_silent(run_noisewalker(genmax = 10, 
                                          timesteps = 10, 
                                          nOctaves = 2, 
                                          frequency = 2.0, 
                                          newSrange = 1.0, 
                                          rep = "10"))

})