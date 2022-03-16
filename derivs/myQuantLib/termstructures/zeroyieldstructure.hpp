//
//  zeroyieldstructure.hpp
//  derivs
//
//  Created by Xin Li on 3/16/22.
//

#ifndef zeroyieldstructure_hpp
#define zeroyieldstructure_hpp

#include "../yieldtermstructure.hpp"
#include <cmath>

namespace myQuantLib {

//! Zero-yield term structure
/*! This abstract class acts as an adapter to YieldTermStructure
    allowing the programmer to implement only the
    <tt>zeroYieldImpl(Time)</tt> method in derived classes.

    Discount and forward are calculated from zero yields.

    Zero rates are assumed to be annual continuous compounding.

    \ingroup yieldtermstructures
*/

class ZeroYieldStructure: public YieldTermStructure {
public:
    // base class Constructors
    explicit ZeroYieldStructure(const DayCounter& dc=DayCounter())
    : YieldTermStructure(dc){}
    explicit ZeroYieldStructure(const Date& ref_date,
                                const Calendar& calendar=Calendar(),
                                const DayCounter& dc=DayCounter(),
                                const std::vector<Handle<Quote>>& jumps = std::vector<Handle<Quote>>(),
                                const std::vector<Date>& jump_dates=std::vector<Date>())
    : YieldTermStructure(ref_date, calendar, dc, jumps, jump_dates) {}
    ZeroYieldStructure(int settlement_days,
                       const Calendar& calendar,
                       const DayCounter& dc=DayCounter(),
                       const std::vector<Handle<Quote>>& jumps=std::vector<Handle<Quote>>(),
                       const std::vector<Date>& jump_dates = std::vector<Date>())
    : YieldTermStructure(settlement_days, calendar, dc, jumps, jump_dates){}
    
protected:
    virtual double zero_yield_impl(double time) const = 0;
    
    double discount_impl(double time) const override {
        // discount is computed by zero yields inputs
        // do not throw when t = 0
        if (time == 0.0) return 1.0;
        double r = zero_yield_impl(time);
        return std::exp(- r * time);
    }
    
};



}


#endif /* zeroyieldstructure_hpp */
