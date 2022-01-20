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
#include "parameters.hpp"

double simpleMC(const VanillaOption& opt,
                double spot,
                const Parameters& vol,
                const Parameters& r,
                unsigned long num_paths){
    // MC price a call option
    // the process: dS = r * S * dt + vol * S * dW
    // terminal S(T) = S * exp( (r - vol*vol/2) * t + vol * sqrt(t) * random_normal(0,1) )
    double var = vol.integrate_square(0.0, opt.get_time_to_exp());
    double sqrvar = std::sqrt(var);
    double s0 = spot * std::exp(r.integrate(0.0, opt.get_time_to_exp()) - var / 2);
    double res = 0;
    for(unsigned long i = 0; i < num_paths; ++i){
        double gau = get_one_gaussian_by_BoxMuller();
        double s = s0 * std::exp(sqrvar * gau);
        res += opt.payoff(s);
    }
    res /= num_paths;
    res *= std::exp(-r.integrate(0.0, opt.get_time_to_exp()));
    
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
    // output both call and put prices
    CallPayoff call_payoff{strike};
    PutPayoff put_payoff{strike};
    VanillaOption call_opt(call_payoff, ttx), put_opt(put_payoff, ttx);
    
    std::cout << "\nEnter spot\n";
    std::cin >> spot;
    std::cout << "\nEnter vol\n";
    std::cin >> vol;
    std::cout << "\ninterest rate r\n";
    std::cin >> r;
    std::cout << "\nthe number of paths\n";
    std::cin >> num_paths;

    ParametersConstant param_vol(vol), param_r(r);
    
    double call_res = simpleMC(call_opt,
                               spot,
                               param_vol,
                               param_r,
                               num_paths
                               );
    double put_res = simpleMC(put_opt,
                              spot,
                              param_vol,
                              param_r,
                              num_paths
                              );
    std::cout << "the price of call option is: " << call_res << "\n";
    std::cout << "the price of put option is:  " << put_res << "\n";
    std::cout << "C - P = " << call_res - put_res <<"\n";
    std::cout << "S - K*exp(-rt)" << spot - strike * std::exp(-param_r.integrate(0, call_opt.get_time_to_exp()));
    
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

