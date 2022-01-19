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

#include <utility>
#include "payoff.hpp"

class VanillaOption {
public:
    VanillaOption(const PayoffBridge& _payoff, double time_to_exp): pf(_payoff), ttx(time_to_exp){}
    //default constructor
    VanillaOption():ttx(0.0), pf(){}
    
    double get_time_to_exp() const {return ttx;}
    void set_time_to_exp(double time_to_exp){ttx = time_to_exp;}
    
    double payoff(double spot) const {return pf(spot);}
    
private:
    PayoffBridge pf;  // use PayoffBridge, we can rely on default big-fives to handle copy/move, used it just like built-in type
    double ttx; // time to expiry
};



#endif /* option_hpp */
