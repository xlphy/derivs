//
//  convergence_tab.cpp
//  derivs
//
//  Created by Xin Li on 1/20/22.
//

#include "convergence_tab.hpp"

ConvergenceTable::ConvergenceTable(const Wrapper<StatsMC>& _inner)
:inner(_inner), results_sofar(0){
    paths_done = 0;
    stop_point = 2;
}

StatsMC* ConvergenceTable::clone() const {
    return new ConvergenceTable(*this);
}

void ConvergenceTable::dump_one_result(double result) {
    inner->dump_one_result(result);
    paths_done++;
    if(paths_done == stop_point){
        // record and move stop_point further
        stop_point *= 2;
        std::vector<std::vector<double>> res(inner->get_results_sofar());
        for(unsigned long i = 0; i < res.size(); ++i){
            res[i].push_back(paths_done);  // add more data, associate current paths_done to whatever the stats so far
            results_sofar.push_back(res[i]);
        }
    }
    return;
}

std::vector<std::vector<double>> ConvergenceTable::get_results_sofar() const {
    std::vector<std::vector<double>> tmp(results_sofar);
    if (paths_done * 2 != stop_point){
        // not the multiple of 2, but add the last result
        std::vector<std::vector<double>> res(inner->get_results_sofar());
        for (unsigned long i = 0; i < res.size(); ++i){
            res[i].push_back(paths_done);
            tmp.push_back(res[i]);
        }
    }
    return tmp;
}



