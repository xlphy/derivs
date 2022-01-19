//
//  main.cpp
//  derivs
//
//  Created by Xin Li on 1/11/22.
//

#include <iostream>
#include <cmath>

#include "random.hpp"
#include "payoff.hpp"
#include "option.hpp"

double simpleMC(const VanillaOption& opt,
                double spot,
                double vol,
                double r,
                unsigned long num_paths){
    // MC price a call option
    // the process: dS = r * S * dt + vol * S * dW
    // terminal S(T) = S * exp( (r - vol*vol/2) * t + vol * sqrt(t) * random_normal(0,1) )
    double var = vol * vol * opt.get_time_to_exp();
    double sqrvar = std::sqrt(var);
    double s0 = spot * std::exp(r * opt.get_time_to_exp() - var / 2);
    double res = 0;
    for(unsigned long i = 0; i < num_paths; ++i){
        double gau = get_one_gaussian_by_BoxMuller();
        double s = s0 * std::exp(sqrvar * gau);
        res += opt.payoff(s);
    }
    res /= num_paths;
    res *= std::exp(-r * opt.get_time_to_exp());
    
    return res;
}


int main(int argc, const char * argv[]) {
    // run simpleMC
    double ttx, strike, spot, vol, r;
    char opt_type;
    unsigned long num_paths;
    std::cout << "\nEnter time to expiry\n";
    std::cin >> ttx;
    std::cout << "\nEnter strike\n";
    std::cin >> strike;
    std::cout << "\nEnter the type of option: call(c) or put(p)";
    std::cin >> opt_type;
    Payoff* payoffptr;
    if(opt_type == 'c')
        payoffptr = new CallPayoff(strike);
    else if(opt_type == 'p')
        payoffptr = new PutPayoff(strike);
    else
        payoffptr = nullptr;
    
    VanillaOption opt(*payoffptr, ttx);
    
    std::cout << "\nEnter spot\n";
    std::cin >> spot;
    std::cout << "\nEnter vol\n";
    std::cin >> vol;
    std::cout << "\ninterest rate r\n";
    std::cin >> r;
    std::cout << "\nthe number of paths\n";
    std::cin >> num_paths;

    double res = simpleMC(opt,
                          spot,
                          vol,
                          r,
                          num_paths);
    std::cout << "the price is " << res << "\n";
    
    double tmp;
    std::cin >> tmp; // wait for an input to exit
    
    delete payoffptr;
    
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

