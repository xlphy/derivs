//
//  path_dependent.hpp
//  derivs
//
//  Created by Xin Li on 1/28/22.
//

#ifndef path_dependent_hpp
#define path_dependent_hpp

#include "mjarray.hpp"
#include <vector>
#include "payoff.hpp"

struct CashFlow{
    double amount;
    unsigned long time_idx;
    
    CashFlow(unsigned long _time_idx = 0UL, double _amount=0.0):time_idx(_time_idx), amount(_amount){};
};

// base class define interface, 3 types of times, they can be different
// 1. lookat_times: times needed to determine payoffs, the times when underlying's prices are required
// 2. cash flows time_idx: the number of cash flows can be different than the number of lookat_times, it is the time that actual cash is paid out and discounting factor is computed. e.g., consider a barrier option, cash flows depending on spot levels, also consider T+2 settlements, the time needed to compute discounting may be different from lookat_times
// 3. all_possible_times: all possible times when the cash flow could happen or all possible times that we need to compute discounting factors
class PathDependent{
public:
    PathDependent(const MJArray& _lookat_times):lookat_times(_lookat_times){};
    
    const MJArray& get_lookat_times() const{return lookat_times;}
    
    // must be implemented by derived concrete classes
    virtual unsigned long max_num_cashflows() const=0;
    virtual MJArray all_possible_times() const=0;
    // spot_values + path_dependent payoffs --> generated cash flows, also return the number generated cashflows
    virtual unsigned long CashFlows(const MJArray& spot_values, std::vector<CashFlow>& generated_flows) const=0;
    virtual PathDependent* clone() const=0;
    
    virtual ~PathDependent(){}
private:
    MJArray lookat_times;
};

class PathDependentAsian: public PathDependent{
public:
    PathDependentAsian(const MJArray& _lookat_times, double _delivery_time, const PayoffBridge& _payoff): PathDependent(_lookat_times),
    delivery_time(_delivery_time), payoff(_payoff), num_times(_lookat_times.size()){}
    
    unsigned long max_num_cashflows() const override {return 1UL;}
    MJArray all_possible_times() const override {
        MJArray tmp(1UL);
        tmp[0] = delivery_time;
        return tmp;
    }
    unsigned long CashFlows(const MJArray& spot_values, std::vector<CashFlow>& generated_flows) const override{
        double mean = spot_values.sum() / num_times;
        generated_flows[0].time_idx = 0UL; // 0 index corresponding to delivery_time
        generated_flows[0].amount = payoff(mean);
        return 1UL;
    }
    PathDependent* clone() const override {return new PathDependentAsian(*this);}
    
private:
    double delivery_time;
    PayoffBridge payoff;
    unsigned long num_times;
    
};



#endif /* path_dependent_hpp */
