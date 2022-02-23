//
//  period.cpp
//  derivs
//
//  Created by Xin Li on 2/22/22.
//

#include "period.hpp"
#include "errors.hpp"

namespace myQuantLib {

std::ostream& operator<<(std::ostream& out, const TimeUnit& u) {
    switch (u) {
        case Years:
            return out << "Years";
        case Months:
            return out << "Months";
        case Weeks:
            return out << "Weeks";
        case Days:
            return out << "Days";
        case Hours:
            return out << "Hours";
        case Minutes:
            return out << "Minutes";
        case Seconds:
            return out << "Seconds";
        case Milliseconds:
            return out << "Milliseconds";
        case Microseconds:
            return out << "Microseconds";
        default:
            myQL_FAIL("unknown TimeUnit");
    }
}

//TODO: implement period functions


}

