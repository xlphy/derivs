//
//  daycounter_simple.cpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#include "daycounter_simple.hpp"
#include "daycounter_thirty360.hpp"

namespace myQuantLib {

namespace {DayCounter fallback = Thirty360(Thirty360::BondBasis);}

Date::serial_type SimpleDayCounter::Impl::day_count(const Date& d1, const Date& d2) const {
    return fallback.day_count(d1, d2);
}

double SimpleDayCounter::Impl::year_fraction(const Date &d1, const Date &d2,
                                             const Date &, const Date &) const {
    Day dm1 = d1.day_of_month(), dm2 = d2.day_of_month();
    if(dm1 == dm2 ||
       (dm1 > dm2 && Date::is_end_of_month(d2)) ||
       (dm1 < dm2 && Date::is_end_of_month(d1))
       ){
        return (d2.year() - d1.year()) + (int(d2.month()) - int(d1.month())) / 12.0;
    }else{
        return fallback.year_fraction(d1, d2);
    }
}




}
