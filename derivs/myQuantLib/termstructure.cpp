//
//  termstructure.cpp
//  derivs
//
//  Created by Xin Li on 3/7/22.
//

#include "termstructure.hpp"
#include "settings.hpp"

namespace myQuantLib {

TermStructure::TermStructure(DayCounter dc)
: _settlement_days(0), _day_counter(std::move(dc)){}

TermStructure::TermStructure(const Date& ref_date, Calendar cal, DayCounter dc)
: _calendar(std::move(cal)), _ref_date(ref_date), _settlement_days(0), _day_counter(std::move(dc)) {}

TermStructure::TermStructure(int settlement_days, Calendar cal, DayCounter dc)
: _moving(true), _updated(false), _calendar(std::move(cal)), _settlement_days(settlement_days), _day_counter(std::move(dc)){
    register_with(Settings::instance().evaluation_date()); // observe evaluation_date()
}

const Date& TermStructure::ref_date() const {
    if(!_updated){
        // trigger update
        Date today = Settings::instance().evaluation_date();
        _ref_date = calendar().advance(today, settlement_days(), Days);
        _updated = true;
    }
    return _ref_date;
}

void TermStructure::update() {
    if(_moving) _updated = false; // trigger recal ref_date when next time it is called
    notify_observers();
}

void TermStructure::check_range(const Date &d, bool extrapolate) const {
    myQL_REQUIRE(d >= ref_date(),
                 "date (" << d << ") before reference date (" << ref_date() << ")");
    myQL_REQUIRE(extrapolate || allow_extrapolation() || d <= max_date(),
                 "date (" << d << ") is past max curve date (" << max_date() << ")");
}


void TermStructure::check_range(double t, bool extrapolate) const {
    myQL_REQUIRE(t >= 0.0, "negative time (" << t << ") given");
    myQL_REQUIRE(extrapolate || allow_extrapolation() || t <= max_time(),
                 "time (" << t << ") is past max curve time (" << max_time() << ")");
}



}
