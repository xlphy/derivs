//
//  main.cpp
//  derivs
//
//  Created by Xin Li on 1/11/22.
//

#include <iostream>
#include <cmath>
#include <cstdlib>

double get_one_uniform(double lower=0.0, double upper=1.0){
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


double simpleMC(double time_to_exp,
                double strike,
                double spot,
                double vol,
                double r,
                unsigned long num_paths){
    // MC price a call option
    // the process: dS = r * S * dt + vol * S * dW
    // terminal S(T) = S * exp( (r - vol*vol/2) * t + vol * sqrt(t) * random_normal(0,1) )
    double var = vol * vol * time_to_exp;
    double sqrvar = std::sqrt(var);
    double s0 = spot * std::exp(r * time_to_exp - var / 2);
    double res = 0;
    for(unsigned long i = 0; i < num_paths; ++i){
        double gau = get_one_gaussian_by_BoxMuller();
        double s = s0 * std::exp(sqrvar * gau);
        double payoff = s > strike? s - strike: 0;
        res += payoff;
    }
    res /= num_paths;
    res *= std::exp(-r * time_to_exp);
    
    return res;
}


int main(int argc, const char * argv[]) {
    // run simpleMC
    double ttx, strike, spot, vol, r;
    unsigned long num_paths;
    std::cout << "\nEnter time to expiry\n";
    std::cin >> ttx;
    std::cout << "\nEnter strike\n";
    std::cin >> strike;
    std::cout << "\nEnter spot\n";
    std::cin >> spot;
    std::cout << "\nEnter vol\n";
    std::cin >> vol;
    std::cout << "\ninterest rate r\n";
    std::cin >> r;
    std::cout << "\nthe number of paths\n";
    std::cin >> num_paths;
    
    double res = simpleMC(ttx,
                          strike,
                          spot,
                          vol,
                          r,
                          num_paths);
    std::cout << "the price is " << res << "\n";
    
    double tmp;
    std::cin >> tmp; // wait for an input to exit

    return 0;
}

/* Identify opportunities to refactor/improve codes
 1. be able to change to different types of payoff, call, put, digital, double digital, etc. --> Payoff class
 2. isolate concept of an option contract, we are really pricing a contract, terms like expiry, strike, payoff should be specified in contract.  --> Option class
 3. market information or modeling parameters like interest rates or vol should be separted out, what if we want a varying interest rate, term structure of vol, --> Parameter class
 4. MC is a simulation, how can I assess its convergence and uncertainty, analyze the statistics. --> OutputStats class
 5. random number generators are important in MC simulator, what if I want to control its behavior (compiler independent) and also tweak around to speed up the convergence --> random number generator
 (additional wish-list)
 6. how can I effectively test our codes? Make sure the new features we add do not break the correctness of program, also test the effect of new codes like speed-ups / improved accuracy, --> write regression tests, unit tests
 7. how can we effectively use our functions, interact with our codes, explore features, and use functions and data to answer our questions --> design user API, modularize, build application-level codes, reproducible / extensible demo andd examples
 8. how can we package / distribute our codes so other people or programms can make use of it, e.g., use by another project, used like an external lib, interface with Excel or Python
 */

