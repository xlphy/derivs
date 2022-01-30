//
//  test.cpp
//  derivs
//
//  Created by Xin Li on 1/28/22.
//

#include "test.hpp"

#include <iostream>
#include <cmath>

#include "random.hpp"
#include "payoff.hpp"
#include "option.hpp"
#include "parameters.hpp"
#include "mcstats.hpp"
#include "wrapper.hpp"
#include "convergence_tab.hpp"
#include "anti_thetic.hpp"
#include "path_dependent.hpp"
#include "exotic_engine.hpp"
#include "tree_product.hpp"
#include "tree.hpp"
#include "BlackScholes.hpp"
#include "func_obj.hpp"
#include "solver.hpp"
#include "factory.hpp"

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

// a generic function that depends on abstract base classes
// extensibility or flexibility is achieved by supplying different concrete derived classes to the function,
// not by changing the script of the function
void simpleMC(const VanillaOption& opt,
              double spot,
              const Parameters& vol,
              const Parameters& r,
              unsigned long num_paths,
              StatsMC& gatherer,
              RandomBase& generator
              )
{
    // MC pricer
    // the process: dS = r * S * dt + vol * S * dW
    // terminal S(T) = S * exp( (r - vol*vol/2) * t + vol * sqrt(t) * random_normal(0,1) )
    double ttx = opt.get_time_to_exp();
    double var = vol.integrate_square(0.0, ttx);
    double sqrvar = std::sqrt(var);
    double s0 = spot * std::exp(r.integrate(0.0, ttx) - var / 2);
    double discounting = std::exp(-r.integrate(0.0, ttx));
    generator.reset_dim(1); // every function call has the same init_seed with the supplied generator
    MJArray variate(1);
    for(unsigned long i = 0; i < num_paths; ++i){
        //double gau = get_one_gaussian_by_BoxMuller();
        generator.get_gaussians(variate);
        double s = s0 * std::exp(sqrvar * variate[0]);
        gatherer.dump_one_result(discounting * opt.payoff(s));
    }
}

// output std::vector<std::vector<double>>
std::ostream& operator<<(std::ostream& out, const std::vector<std::vector<double>>& res){
    for(unsigned long i = 0; i < res.size(); ++i){
        for(unsigned long j = 0; j < res[i].size(); ++j)
            out << res[i][j] << ", ";
        out << "\n";
    }
    return out;
}

// output std::vector<T>
template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& res){
    for(const auto& e: res)
        out << e << ", ";
    out << "\n";
    return out;
}

// read inputs
template<typename T>
void read_input(const std::string& prompt, T& variable){
    std::cout << prompt;
    std::cin >> variable;
}

void test_simpleMC(){
    // run simpleMC
    // read raw input
    double ttx, strike, spot, vol, r;
    unsigned long num_paths;
    std::cout << "\nEnter time to expiry\n";
    std::cin >> ttx;
    std::cout << "\nEnter strike\n";
    std::cin >> strike;
    // payoff and option objects
    CallPayoff call_payoff{strike};
    PutPayoff put_payoff{strike};
    VanillaOption call_opt(call_payoff, ttx), put_opt(put_payoff, ttx);
    // read raw input
    std::cout << "\nEnter spot\n";
    std::cin >> spot;
    std::cout << "\nEnter vol\n";
    std::cin >> vol;
    std::cout << "\ninterest rate r\n";
    std::cin >> r;
    std::cout << "\nthe number of paths\n";
    std::cin >> num_paths;
    // parameters object
    ParametersConstant param_vol(vol), param_r(r);
    // gather for collecting statistics and results
    StatsMean gatherer_call, gatherer_put;
    // check convergence
    ConvergenceTable gatherer2(gatherer_call); // decorator pattern, same input but slightly changed behaviors and outputs
    RandomParkMiller generator(1, 1);  // set dim and ini_seed, same init_seed->same result
    AntiThetic generator2 = AntiThetic(generator); // using anti-thetic sampling
    ConvergenceTable gatherer3(gatherer_call);
    // run MC simulation
    simpleMC(call_opt, spot, param_vol, param_r, num_paths, gatherer_call, generator);
    simpleMC(put_opt, spot, param_vol, param_r, num_paths, gatherer_put, generator);
    simpleMC(call_opt, spot, param_vol, param_r, num_paths, gatherer2, generator);
    simpleMC(call_opt, spot, param_vol, param_r, num_paths, gatherer3, generator2);
    
    // output results
    std::vector<std::vector<double>> res = gatherer_call.get_results_sofar();
    std::cout << "Call results:\n";
    std::cout << res;
    double call_price = res[0][0];
    res = gatherer_put.get_results_sofar();
    std::cout << "Put results:\n";
    std::cout << res;
    double put_price = res[0][0];
    
    std::cout << "C - P = " << call_price- put_price <<"\n";
    std::cout << "S - K*exp(-rt) = " << spot - strike * std::exp(-param_r.integrate(0, call_opt.get_time_to_exp())) << "\n";
    std::cout << "Convergence Table: \n";
    res = gatherer2.get_results_sofar();
    std::cout << res;
    std::cout << "Convergence Table (use anti-thetic sampling): \n";
    res = gatherer3.get_results_sofar();
    std::cout << res;
}

void test_exoticEngine(){
    double ttx, strike, spot, vol, r, div;
    unsigned long num_paths, num_dates;
    
    std::cout <<"pricing an Asian call option\n";
    read_input<double>("Enter time to expiry: ", ttx);
    read_input<double>("Strike: ", strike);
    read_input<double>("Spot: ", spot);
    read_input<double>("vol: ", vol);
    read_input<double>("r: ", r);
    read_input<double>("dividend: ", div);
    read_input<unsigned long>("number of dates: ", num_dates);
    read_input<unsigned long>("number of paths: ", num_paths);
    
    
    CallPayoff payoff(strike);
    MJArray times(num_dates);
    for(unsigned long i=0; i<num_dates; ++i)
        times[i] = (i + 1.0) * ttx / num_dates;
    ParametersConstant vol_param(vol), rate_param(r), div_param(div);
    PathDependentAsian opt(times, ttx, payoff);
    StatsMean gatherer;
    ConvergenceTable gatherer_tab(gatherer);
    RandomParkMiller generator(num_dates);
    AntiThetic gen_two(generator);
    
    ExoticBSEngine engine(opt, rate_param, div_param, vol_param,
                          gen_two, spot);
    engine.run_simulation(gatherer_tab, num_paths);
    
    std::vector<std::vector<double>> res = gatherer_tab.get_results_sofar();
    std::cout << "Results for prices are: \n";
    std::cout << res;
    
    return;
}

void test_tree(){
    double ttx, strike, spot, vol, r, div;
    unsigned long steps;
    
    std::cout <<"pricing by tree\n";
    read_input<double>("Enter time to expiry: ", ttx);
    read_input<double>("Strike: ", strike);
    read_input<double>("Spot: ", spot);
    read_input<double>("vol: ", vol);
    read_input<double>("r: ", r);
    read_input<double>("dividend: ", div);
    read_input<unsigned long>("number of steps: ", steps);
    
    CallPayoff payoff(strike);
    ParametersConstant r_param(r);
    ParametersConstant div_param(div);
    
    TreeEuropean euro_opt(ttx, payoff);
    TreeAmerican am_opt(ttx, payoff);
    
    BinomialTree tree(spot, r_param, div_param, vol, steps, ttx);
    double euro_price = tree.get_price(euro_opt);
    double am_price = tree.get_price(am_opt);
    std::cout << "euro/am call tree price: " << euro_price << ", " << am_price << "\n";
    // compare with Black-Scholes
    double bs_price = bs_call(spot, strike, r, div, vol, ttx);
    std::cout << "BS formula euro price: " << bs_price << "\n";
    ForwardPayoff fwd_payoff(strike);
    TreeEuropean forward(ttx, fwd_payoff);
    double fwd_price = tree.get_price(forward);
    std::cout << "forward price by tree: " << fwd_price << "\n";
    double fwd0 = std::exp(-r*ttx) * (spot * std::exp((r-div)*ttx) - strike);
    std::cout << "actual forward price: " << fwd0 << "\n";
    
    // redo tree price with one more step
    steps++;
    BinomialTree tree2(spot, r_param, div_param, vol, steps, ttx);
    double euro_price2 = tree2.get_price(euro_opt);
    double am_price2 = tree2.get_price(am_opt);
    std::cout << "euro/am call tree2 price: " << euro_price2 <<", " << am_price2 << "\n";
    double fwd_price2 = tree2.get_price(forward);
    std::cout << "forward price by tree2: " << fwd_price2 << "\n";
    double euro_avg = 0.5 * (euro_price + euro_price2);
    double am_avg = 0.5 * (am_price + am_price2);
    double fwd_avg = 0.5 * (fwd_price + fwd_price2);
    std::cout << "average prices: \neuro/am call avg price: " << euro_avg << ", " << am_avg << "\n";
    std::cout << "forward avg price: " << fwd_avg << "\n";
}

void test_solver(){
    double ttx, strike, spot, vol, r, div, price;
    
    std::cout <<"Solve for implied Vol\n";
    read_input<double>("Enter time to expiry: ", ttx);
    read_input<double>("Strike: ", strike);
    read_input<double>("Spot: ", spot);
    read_input<double>("r: ", r);
    read_input<double>("dividend: ", div);
    read_input("input a call price: ", price);
    
    std::cout << "Solve by Bisection:\n";
    double low, high, tol;
    read_input("lower estimate: ", low);
    read_input("higher estimate: ", high);
    read_input("error tolerance: ", tol);
    BSCall call_func{r, div, ttx, spot, strike};
    vol = bisection(price, low, high, tol, call_func);
    double price2 = bs_call(spot, strike, r, div, vol, ttx);
    std::cout << "solved vol = " << vol << ", reprice = " << price2 << "\n";
    
    std::cout << "Solve by Newton-Raphson:\n";
    double init_vol;
    read_input("start by an initiall guess vol: ", init_vol);
    read_input("error tolerance: ", tol);
    BSCall2 call_func2(r, div, ttx, spot, strike);
    vol = newton<BSCall2, &BSCall2::price, &BSCall2::vega>(price, init_vol, tol, call_func2);
    price2 = bs_call(spot, strike, r, div, vol, ttx);
    std::cout << "solved vol = " << vol << ", reprice = " << price2 << "\n";
    
}

void test_factory(){
    std::cout << "test payoff factory\n";
    
    double strike;
    std::string name;
    read_input("enter strike: ", strike);
    read_input("enter payoff name: ", name);
    
    // get the singleton instance, can pass by-reference but not by-value
    PayoffFactory& factory = PayoffFactory::instance();
    Payoff* ptr = factory.create_payoff(name, strike);
    if (ptr != nullptr){
        // find it
        double spot;
        read_input("payoff found, please input a spot to check payoff function\nspot:", spot);
        std::cout << "payoff(" << spot << ") = " << ptr->operator()(spot) << "\n";
    }else{
        // not found
        std::cout << "unrecognized name, please input a registered name!\n";
        std::vector<std::string> all_names = factory.get_all_ids();
        std::cout << "all registered names are: " << all_names;
    }
    delete ptr;
}


