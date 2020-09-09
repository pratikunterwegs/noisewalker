#include <iostream>
#include <vector>
#include <cmath>
#include "rndutils.hpp"
#include "ann2.hpp"


using namespace ann;


using ANN = Network<float,
  Layer<Neuron<2, activation::sgn::unipolar>, 2>,
  Layer<Neuron<2, activation::sgn::unipolar>, 1>
>;


float test_binary_op(ANN& ann)
{
  float abserr = 0.f;
  abserr += std::abs(0 - ann(0, 0)[0]);
  abserr += std::abs(1 - ann(0, 1)[0]);
  abserr += std::abs(1 - ann(1, 0)[0]);
  abserr += std::abs(0 - ann(1, 1)[0]);
  return abserr;
}


const size_t G = 10000;
const size_t N = 10000;
std::vector<ANN> pop(N);
std::vector<ANN> tmp(N);
std::vector<float> fitness(N);
auto mu_dist = std::bernoulli_distribution(0.01);
auto mu_w_dist = std::cauchy_distribution<float>(0.f, 0.01f);
auto fitness_dist = rndutils::mutable_discrete_distribution<int, rndutils::all_zero_policy_uni>{};

int main()
{
  auto reng = rndutils::make_random_engine<>();
  for (size_t g = 0; g < G; ++g) {
    for (size_t i = 0; i < N; ++i) {
      fitness[i] = 4.f - test_binary_op(pop[i]);
    }
    auto m = std::max_element(fitness.cbegin(), fitness.cend());
    std::cout << g << "  " << *m << '\n';
    if (*m == 4.f) {
      break;
    }
    fitness_dist.mutate(fitness.cbegin(), fitness.cend());
    for (size_t i = 0; i < N; ++i) {
      tmp[i] = pop[fitness_dist(reng)];
    }
    for (auto& ann : tmp) {
      for (auto& w : ann) {
        if (mu_dist(reng)) {
          w += mu_w_dist(reng);
        }
      }
    }
    tmp.swap(pop);
  }
  return 0;
}
