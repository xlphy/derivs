//
//  random.cpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#include "random.hpp"
#include <cstdlib>  // rand
#include <cmath>
#include "normals.hpp"


void RandomBase::get_gaussians(MJArray &variates){
    get_uniforms(variates);
    for(unsigned long i=0; i<dim; ++i){
        double x = variates[i];
        variates[i] = inv_cum_norm(x);
    }
}

constexpr long PM_A = 16807;
constexpr long PM_M = 2147483647;
constexpr long PM_Q = 127773;
constexpr long PM_R = 2836;

ParkMiller::ParkMiller(long _seed) : seed(_seed){
    if(seed==0) seed=1;
}

void ParkMiller::set_seed(long _seed){
    seed = _seed;
    if(seed==0) seed=1;
}

unsigned long ParkMiller::Max(){return PM_M - 1;}
unsigned long ParkMiller::Min(){return 1;}

long ParkMiller::get_one_integer(){
    long k;
    k = seed / PM_Q;
    
    seed = PM_A * (seed - k * PM_Q) - PM_R * k;
    if(seed<0) seed += PM_M;
    
    return seed;
}



double get_one_uniform(double lower, double upper){
    // return one random variable from uniform dist. [lower, upper]
    return lower + (upper - lower) * std::rand() / static_cast<double>(RAND_MAX);
}

double get_one_gaussian_by_BoxMuller(){
    // generate a random number from standard normal dist.
    // Box-Muller transformation: https://en.wikipedia.org/wiki/Box–Muller_transform
    // exp(-r^2/2) ~ U1; phi/2pi ~ U2; U ~ [0, 1]
    // use polar form
    // get one uniform u1 = r2 = u*u + v*v; cos(2pi * U2) = u / sqrt(r2)
    double r2, u, v;
    do{
        u = get_one_uniform(-1, 1);
        v = get_one_uniform(-1, 1);
        r2 = u*u + v*v;
    }while(r2 >= 1.0);
    return u * std::sqrt(-2 * std::log(r2) / r2);
}

double get_one_gaussian_by_sum(){
    // generate a random number by a sum, invoking CLT
    // the sum of 12 uniform random variables, mean = 6, var = 12 * 1 / 12
    double res = 0.0;
    for(int i=0; i<12; ++i)
        res += get_one_uniform();
    return res - 6.0;
}
