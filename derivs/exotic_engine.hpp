//
//  exotic_engine.hpp
//  derivs
//
//  Created by Xin Li on 1/28/22.
//

#ifndef exotic_engine_hpp
#define exotic_engine_hpp

/*A generic exotic pricer engine, abstract exotic pricing workflow, separation of four generic steps:
 1. The generation of the stock price path
 2. The generation of cash-flows given a stock price path and exotic option payoff specs
 3. The discounting and summing of cash-flows for a given path
 4. The average (more generic statistics) of the prices over all the paths
 */

#include "wrapper.hpp"
#include "path_dependent.hpp"
#include "parameters.hpp"
#include "mcstats.hpp"

#include "random.hpp"

class ExoticEngine{
public:
    ExoticEngine(const Wrapper<PathDependent>& product_, const Parameters& r_)
    :product(product_), r(r_), discounts(product_->all_possible_times()){
        for(unsigned long i=0; i < discounts.size(); ++i)
            discounts[i] = std::exp(-r.integrate(0.0, discounts[i]));
    }
    
    void run_simulation(StatsMC& result_gatherer, unsigned long num_paths){
        MJArray spot_values(product->get_lookat_times().size());
        cash_flows.resize(product->max_num_cashflows());
        // run simulation and collect results
        double val;
        for(unsigned long i=0; i<num_paths; ++i){
            get_one_path(spot_values);
            val = do_one_path(spot_values);
            result_gatherer.dump_one_result(val);
        }
        return;
    }
    double do_one_path(const MJArray& spot_values) const{
        // accounting along one path
        unsigned long num_cashflows = product->CashFlows(spot_values, cash_flows);
        double val = 0.0;
        for(unsigned i=0; i<num_cashflows; ++i)
            // make sure cash_flows and discounts time (cash_flows[i].time_idx) are aligned
            val += cash_flows[i].amount * discounts[cash_flows[i].time_idx];
        return val;
    }
    
    virtual void get_one_path(MJArray& spot_values) = 0; // generate spot_values by a stochastic process
    virtual ~ExoticEngine(){}
    
protected:
    Wrapper<PathDependent> product;
private:
    Parameters r; // interest rate
    MJArray discounts;
    mutable std::vector<CashFlow> cash_flows;  // can be modified in a const member function, not really a data member but it is a workspace which is created onece and for all (inherited classes) at the beginning,
};



class ExoticBSEngine: public ExoticEngine{
public:
    ExoticBSEngine(const Wrapper<PathDependent>& _product,
                   const Parameters& _r,  // interest rate
                   const Parameters& _d,  // dividends
                   const Parameters& _vol,
                   const Wrapper<RandomBase>& _generator,
                   double _spot0
                   );
    void get_one_path(MJArray& spot_values) override;

private:
    Wrapper<RandomBase> generator;
    MJArray drifts;
    MJArray stds;
    double log_spot;
    unsigned long num_times;
    MJArray variates;
};


#endif /* exotic_engine_hpp */
