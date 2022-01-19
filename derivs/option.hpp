//
//  option.hpp
//  derivs
//
//  Created by Xin Li on 1/16/22.
//

#ifndef option_hpp
#define option_hpp

/*The abstraction of an opiton contract which contains Payoff function and Expiry
 VanillaOption:
 1) Payoff at Expiry, a function of terminal spot, which may depend on parameters
 2) Time to Expiry, the time until option contract settled
 */

#include "payoff.hpp"

class VanillaOption {
public:
    VanillaOption(const Payoff& payoff, double time_to_exp): ttx(time_to_exp){
        // make a copy of payoff
        payoffptr = payoff.clone();
    }
    
    double get_time_to_exp() const {return ttx;}
    void set_time_to_exp(double time_to_exp){ttx = time_to_exp;}
    
    double payoff(double spot) const {return (*payoffptr)(spot);}
    
    ~VanillaOption(){delete payoffptr;}
private:
    Payoff* payoffptr;  // payoff at expiry, use a pointer because we want to be generic
    double ttx; // time to expiry
};



#endif /* option_hpp */
