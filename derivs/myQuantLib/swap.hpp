//
//  swap.hpp
//  derivs
//
//  Created by Xin Li on 2/15/22.
//

/* An example of a concrete instrument class, interest-rate swap*/

#ifndef swap_hpp
#define swap_hpp

#include "instrument.hpp"
#include <vector>

namespace myQuantLib {

//TODO: include more types, CashFlow, Handle, YieldTermStructure

class Swap : public Instrument {
public:
    Swap(const std::vector<std::shared_ptr<CashFlow>>& first_leg,
         const std::vector<std::shared_ptr<CashFlow>>& second_leg,
         const Handle<YieldTermStructure>& term_struct);
    bool is_expired() const override;
    double first_leg_bps() const;
    double second_leg_bps() const;
protected:
    // methods
    void setup_expired() const override;
    void do_calculation() const override;
    // data
    std::vector<std::shared_ptr<CashFlow>> _first_leg, _second_leg;
    Handle<YieldTermStructure> _term_struct;
    mutable double _first_leg_bps, _second_leg_bps; // in addition to _npv, more results to be saved.
};

}





#endif /* swap_hpp */
