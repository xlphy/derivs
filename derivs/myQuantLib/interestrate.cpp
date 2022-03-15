//
//  interestrate.cpp
//  derivs
//
//  Created by Xin Li on 3/13/22.
//

#include "interestrate.hpp"
#include <cmath>


namespace myQuantLib {

double InterestRate::compound_factor(double time) const {
    myQL_REQUIRE(time >= 0.0, "negative time (" << time << ") not allowed");
    switch (_comp) {
        case Simple:
            return 1.0 * _r * time;
        case Compounded:
            return std::pow(1.0 + _r / _freq, _freq * time);
        case Continuous:
            return std::exp(_r * time);
        case SimpleThenCompounded:
            if (time <= 1.0 / double(_freq))
                return 1.0 + _r * time;
            else
                return std::pow(1.0 + _r / _freq, _freq * time);
        case CompoundedThenSimple:
            if (time > 1.0 / double(_freq))
                return 1.0 + _r * time;
            else
                return std::pow(1.0 + _r / _freq, _freq * time);
        default:
            myQL_FAIL("unknown compounding convention");
    }
}

InterestRate InterestRate::implied_rate(double compound,
                                        const DayCounter& resultDC,
                                        Compounding comp,
                                        Frequency freq,
                                        double time){
    myQL_REQUIRE(compound > 0.0, "positive compound factor required");
    // infer rate given inputs
    double r;
    if (compound == 1.0){
        myQL_REQUIRE(time >= 0.0, "non negative time (" << time << ") required");
        r = 0.0;
    } else {
        myQL_REQUIRE(time > 0.0, "positive time (" << time << ") required");
        switch (comp) {
            case Simple:
                r = (compound - 1.0) / time;
                break;
            case Compounded:
                r = (std::pow(compound, 1.0 / (double(freq) * time)) - 1.0) * double(freq);
                break;
            case Continuous:
                r = std::log(compound) / time;
                break;
            case SimpleThenCompounded:
                if (time < 1.0 / double(freq))
                    r = (compound - 1.0) / time;
                else
                    r = (std::pow(compound, 1.0 / (double(freq) * time)) - 1.0) * double(freq);
                break;
            case CompoundedThenSimple:
                if (time > 1.0 / double(freq))
                    r = (compound - 1.0) / time;
                else
                    r = (std::pow(compound, 1.0 / (double(freq) * time)) - 1.0) * double(freq);
                break;
            default:
                myQL_FAIL("unknown compounding convention (" << int(comp) << ")");
        }
    }
    return InterestRate(r, resultDC, comp, freq);
}

std::ostream& operator<<(std::ostream& out, const InterestRate& ir) {
    out << ir.rate() << " " << ir.day_counter().name() << " ";
    switch (ir.compounding()){
        case Simple:
            out << "simple compounding";
            break;
        case Compounded:
            switch(ir.frequency()){
                case NoFrequency:
                case Once:
                    myQL_FAIL(ir.frequency() << " frequency not allowed for this interest rate");
                default:
                    out << ir.frequency() << " compounding";
            }
            break;
        case Continuous:
            out << "continuous compounding";
            break;
        case SimpleThenCompounded:
            switch (ir.frequency()) {
              case NoFrequency:
              case Once:
                myQL_FAIL(ir.frequency() << " frequency not allowed for this interest rate");
              default:
                out << "simple compounding up to "
                    << int(12 / ir.frequency()) << " months, then "
                    << ir.frequency() << " compounding";
            }
            break;
        case CompoundedThenSimple:
            switch (ir.frequency()) {
              case NoFrequency:
              case Once:
                myQL_FAIL(ir.frequency() << " frequency not allowed for this interest rate");
              default:
                out << "compounding up to "
                    << int(12 / ir.frequency()) << " months, then "
                    << ir.frequency() << " simple compounding";
            }
            break;
        default:
            myQL_FAIL("unknown compounding convention (" << int(ir.frequency()) << ")");
    }
    return out;
}


}
