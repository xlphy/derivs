//
//  forwardratestructure.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef forwardratestructure_hpp
#define forwardratestructure_hpp

#include "../yieldtermstructure.hpp"
#include <cmath>

namespace myQuantLib {

//! %Forward-rate term structure
/*! This abstract class acts as an adapter to YieldTermStructure allowing
    the programmer to implement only the <tt>forwardImpl(Time)</tt> method
    in derived classes.

    Zero yields and discounts are calculated from forwards.

    Forward rates are assumed to be annual continuous compounding.

    \ingroup yieldtermstructures
*/

class ForwardRateStructure : public YieldTermStructure {
public:
    explicit ForwardRateStructure(const DayCounter& dayCounter=DayCounter())
    : YieldTermStructure(dayCounter){}
    explicit ForwardRateStructure(const Date& ref_date,
                                  const Calendar& cal=Calendar(),
                                  const DayCounter& dayCounter=DayCounter(),
                                  const std::vector<Handle<Quote>>& jumps=std::vector<Handle<Quote>>(),
                                  const std::vector<Date>& jump_dates=std::vector<Date>())
    : YieldTermStructure(ref_date, cal, dayCounter, jumps, jump_dates){}
    ForwardRateStructure(int settlement_days,
                         const Calendar& cal,
                         const DayCounter& dayCounter = DayCounter(),
                         const std::vector<Handle<Quote> >& jumps = std::vector<Handle<Quote> >(),
                         const std::vector<Date>& jump_dates = std::vector<Date>())
    : YieldTermStructure(settlement_days, cal, dayCounter, jumps, jump_dates){}
protected:
    virtual double forward_impl(double time) const = 0;
    
    virtual double zero_yield_impl(double time) const {
        // provide default zero_yield_impl based on forward_impl
        if (time == 0.0) return forward_impl(0.0);
        // implement smarter integration if plan to use the following code
        // provide a basic integration method here
        double sum = 0.5 * forward_impl(0.0);
        std::size_t n = 1000;
        double dt = time / n;
        for (double i=dt; i<time; i+=dt)
            sum += forward_impl(i);
        sum += 0.5 * forward_impl(time);
        return sum * dt / time;
    }
    
    // provide override for base class
    double discount_impl(double time) const override {
        if(time == 0.0) return 1.0;
        double r = zero_yield_impl(time);
        return std::exp(- r * time);
    }
};



}


#endif /* forwardratestructure_hpp */
