//
//  mcstats.hpp
//  derivs
//
//  Created by Xin Li on 1/19/22.
//

#ifndef mcstats_hpp
#define mcstats_hpp

/*class that deals with various statistics from MC simulations*/
#include <vector>

//base class, define interface, collect one result
class StatsMC{
public:
    // base-stub
    StatsMC(){}
    virtual StatsMC* clone() const = 0;
    virtual ~StatsMC(){}
    
    // base-interfaces
    virtual void dump_one_result(double result) = 0;
    virtual std::vector<std::vector<double>> get_results_sofar() const = 0;
    
private:
    // no data members, define interfaces
};

class StatsMean: public StatsMC{
public:
    StatsMean(): running_sum(0), paths_done(0ul){}
    StatsMC* clone() const override {return new StatsMean(*this);}
    
    void dump_one_result(double result) override {
        running_sum += result;
        paths_done += 1;
    }
    std::vector<std::vector<double>> get_results_sofar() const override {
        std::vector<std::vector<double>> ret(1);
        ret[0].resize(1);
        if(paths_done != 0)
            ret[0][0] = running_sum / paths_done;
        return ret;
    }

private:
    double running_sum;
    unsigned long paths_done;
};



#endif /* mcstats_hpp */
