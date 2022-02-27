//
//  daycounter_simple.hpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#ifndef daycounter_simple_hpp
#define daycounter_simple_hpp

#include "daycounter.hpp"

namespace myQuantLib {
//! Simple day counter for reproducing theoretical calculations.
/*! This day counter tries to ensure that whole-month distances
    are returned as a simple fraction, i.e., 1 year = 1.0,
    6 months = 0.5, 3 months = 0.25 and so forth.
 */

class SimpleDayCounter : public DayCounter {
private:
    class Impl : public DayCounter::Impl {
    public:
        std::string name() const override {return "Simple";}
        Date::serial_type day_count(const Date& d1, const Date& d2) const override;
        double year_fraction(const Date& d1, const Date& d2, const Date&, const Date&) const override;
    };
public:
    SimpleDayCounter():
    // construct with derived DayCounter Impl
    DayCounter(std::shared_ptr<DayCounter::Impl>(new SimpleDayCounter::Impl())){}
};


}


#endif /* daycounter_simple_hpp */
