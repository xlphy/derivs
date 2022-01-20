//
//  convergence_tab.hpp
//  derivs
//
//  Created by Xin Li on 1/20/22.
//

#ifndef convergence_tab_hpp
#define convergence_tab_hpp

/*check convergence of any statistics for each MC simulation run*/
#include <vector>
#include "mcstats.hpp"
#include "wrapper.hpp"

class ConvergenceTable: public StatsMC{
public:
    ConvergenceTable(const Wrapper<StatsMC>& _inner);
    StatsMC* clone() const override;
    
    void dump_one_result(double result) override;
    std::vector<std::vector<double>> get_results_sofar() const override;
    
private:
    Wrapper<StatsMC> inner; // a specific stats to check for convergence
    std::vector<std::vector<double>> results_sofar;
    unsigned long stop_point;
    unsigned long paths_done;
};



#endif /* convergence_tab_hpp */
