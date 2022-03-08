//
//  termstructure.cpp
//  derivs
//
//  Created by Xin Li on 3/7/22.
//

#include "termstructure.hpp"

namespace myQuantLib {

TermStructure::TermStructure(DayCounter dc)
: _settlement_days(0), _day_counter(std::move(dc)){}

TermStructure::TermStructure(const Date& ref_date, Calendar cal, DayCounter dc)
: _calendar(std::move(cal)), _ref_date(ref_date), _settlement_days(0), _day_counter(std::move(dc)) {}

TermStructure::TermStructure(int settlement_days, Calendar cal, DayCounter dc)
: _moving(true), _update(false), _calendar(std::move(cal)), _settlement_days(settlement_days), _day_counter(std::move(dc)){
    //TODO: Settings, register with evaluation_date
}



}
