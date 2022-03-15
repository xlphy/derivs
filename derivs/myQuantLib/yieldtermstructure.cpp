//
//  yieldtermstructure.cpp
//  derivs
//
//  Created by Xin Li on 3/13/22.
//

#include "yieldtermstructure.hpp"

namespace myQuantLib {

//TODO: implement discount, zero_rate, forward_rate and set_jumps

namespace {
    // time interval used in finite difference
    const double dt = 0.0001;  // 0.0001 * 365.25 * 24 = 0.88 hour
}

void YieldTermStructure::set_jumps(const Date &ref_date) {
    if(_jump_dates.empty() && !_jumps.empty()) {
        // year-end dates
        _jump_dates.resize(n_jumps);
        _jump_times.resize(n_jumps);
        Year y = ref_date.year();
        for(std::size_t i=0; i < n_jumps; ++i)
            _jump_dates[i] = Date(31, December, y + i);
    } else {
        // fixed dates
        myQL_REQUIRE(_jump_dates.size() == n_jumps,
                     "mismatch between number of jumps (" << n_jumps
                     << ") and jump dates (" << _jump_dates.size() << ")");
    }
    for (std::size_t i=0; i<n_jumps; ++i)
        _jump_times[i] = time_from_ref(_jump_dates[i]);
    _latest_ref = ref_date;
}

double YieldTermStructure::discount(double time, bool extrapolate) const {
    check_range(time, extrapolate);
    if(_jumps.empty())
        return discount_impl(time);
    
    double jump_effect = 1.0;
    for(std::size_t i=0; i<n_jumps; ++i) {
        if (_jump_times[i] > 0 && _jump_times[i] < time) {
            myQL_REQUIRE(_jumps[i]->is_valid(), "invalid " << (i+1) << "-th jump quote");
            double this_jump = _jumps[i]->value();
            myQL_REQUIRE(this_jump > 0.0, "invalid " << (i+1) << "-th jump value: " << this_jump);
            jump_effect *= this_jump;
        }
    }
    return jump_effect * discount_impl(time);
}


InterestRate YieldTermStructure::zero_rate(const Date& d,
                                           const DayCounter& dc,
                                           Compounding comp,
                                           Frequency freq,
                                           bool extrapolate) const {
    if(d==ref_date()){
        // adding a small dt amount
        double compound = 1.0 / discount(dt, extrapolate);
        // t has been calculated with a possibly different daycounter
        // but the difference should not matter for very small times
        return InterestRate::implied_rate(compound, dc, comp, freq, dt);
    }
    double compound = 1.0 / discount(d, extrapolate);
    return InterestRate::implied_rate(compound, dc, comp, freq, ref_date(), d);
}

InterestRate YieldTermStructure::zero_rate(double time,
                                           Compounding comp,
                                           Frequency freq,
                                           bool extrapolate) const {
    if (time==0.0) time = dt;
    double compound = 1.0 / discount(time, extrapolate);
    return InterestRate::implied_rate(compound, day_counter(), comp, freq, time);
}

InterestRate YieldTermStructure::forward_rate(const Date& d1,
                                              const Date& d2,
                                              const DayCounter& dc,
                                              Compounding comp,
                                              Frequency freq,
                                              bool extrapolate) const {
    if (d1 == d2){
        check_range(d1, extrapolate);
        double t1 = std::max(time_from_ref(d1) - dt / 2.0, 0.0);
        double t2 = t1 + dt;
        double compound = discount(t1, true) / discount(t2, true);
        // times have been calculated with a possibly different daycounter
        // but the difference should not matter for very small times
        return InterestRate::implied_rate(compound, dc, comp, freq, dt);
    }
    myQL_REQUIRE(d1 < d2, d1 << " later than " << d2);
    double compound = discount(d1, extrapolate) / discount(d2, extrapolate);
    return InterestRate::implied_rate(compound, dc, comp, freq, d1, d2);
}


InterestRate YieldTermStructure::forward_rate(double t1, double t2,
                                              Compounding comp,
                                              Frequency freq,
                                              bool extrapolate) const {
    double compound;
    if (t2 == t1) {
        check_range(t1, extrapolate);
        t1 = std::max(t1 - dt/2.0, 0.0);
        t2 = t1 + dt;
        compound = discount(t1, true) / discount(t2, true);
    } else {
        myQL_REQUIRE(t2 > t1, "t2 (" << t2 << ") < t1 (" << t2 << ")");
        compound = discount(t1, extrapolate) / discount(t2, extrapolate);
    }
    return InterestRate::implied_rate(compound, day_counter(), comp, freq, t2 - t1);
}

void YieldTermStructure::update() {
    TermStructure::update();
    Date new_ref= Date();
    try {
        new_ref = ref_date();
        if (new_ref != _latest_ref)
            set_jumps(new_ref);
    } catch (Error&){
        if(new_ref == Date()){
            // the curve couldn't calculate the reference
            // date. Most of the times, this is because some
            // underlying handle wasn't set, so we can just absorb
            // the exception and continue; the jumps will be set
            // correctly when a valid underlying is set.
            return;
        } else {
            // something else happened during the call to
            // setJumps(), so we let the exception bubble up.
            throw;
        }
    }
}

}
