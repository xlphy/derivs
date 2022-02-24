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


std::ostream& operator<<(std::ostream& out, Frequency f){
    switch (f) {
      case NoFrequency:
        return out << "No-Frequency";
      case Once:
        return out << "Once";
      case Annual:
        return out << "Annual";
      case Semiannual:
        return out << "Semiannual";
      case EveryFourthMonth:
        return out << "Every-Fourth-Month";
      case Quarterly:
        return out << "Quarterly";
      case Bimonthly:
        return out << "Bimonthly";
      case Monthly:
        return out << "Monthly";
      case EveryFourthWeek:
        return out << "Every-fourth-week";
      case Biweekly:
        return out << "Biweekly";
      case Weekly:
        return out << "Weekly";
      case Daily:
        return out << "Daily";
      case OtherFrequency:
        return out << "Unknown frequency";
      default:
        myQL_FAIL("unknown frequency (" << int(f) << ")");
    }
}

Period::Period(Frequency f) {
    switch (f) {
        case NoFrequency:
            _units = Days;
            _len = 0;
            break;
        // in Years
        case Once:
            _units = Years;
            _len = 0;
            break;
        case Annual:
            _units = Years;
            _len = 1;
            break;
        // in Months
        case Semiannual: // f = 2
        case EveryFourthMonth: // f = 3
        case Quarterly: // f = 4
        case Bimonthly: // f = 6
        case Monthly: // f = 12
            _units = Months;
            _len = 12 / f;
            break;
        // in Weeks
        case EveryFourthWeek: // f = 13
        case Biweekly: // f = 26
        case Weekly: // f = 52
            _units = Weeks;
            _len = 52 / f;
            break;
        // in Days
        case Daily:
            _units = Days;
            _len = 1;
            break;
        case OtherFrequency:
            myQL_FAIL("unknown frequency");
        default:
            myQL_FAIL("unknown frequency (" << int(f) << ")");
    }
}

Frequency Period::frequency() const {
    std::size_t len = std::abs(_len);
    
    if(len == 0){
        if(_units == Years) return Once;
        return NoFrequency;
    }
    switch (_units) {
        case Years:
            if(len == 1)
                return Annual;
            else
                return OtherFrequency;
        case Months:
            if(12%len == 0 && len <= 12)
                return Frequency(12/len);
            else
                return OtherFrequency;
        case Weeks:
            if(len==1)
                return Weekly;
            else if (len==2)
                return Biweekly;
            else if (len==4)
                return EveryFourthWeek;
            else
                return OtherFrequency;
        case Days:
            if(len==1)
                return Daily;
            else
                return OtherFrequency;
        default:
            myQL_FAIL("unknown time unit (" << int(_units) << ")");
    }
}

void Period::normalize() {
    if(_len != 0)
        switch (_units) {
            case Months:
                if((_len % 12) == 0){
                    _len /= 12;
                    _units = Years;
                }
                break;
            case Days:
            case Weeks:
            case Years:
                break;
            default:
                myQL_FAIL("unknown time unit (" << int(_units) << ")");
        }
}

Period& Period::operator+=(const Period& p){
    if(_len==0){
        _len = p.length();
        _units = p.units();
    }else if (_units == p.units()){
        // no conversion needed
        _len += p.length();
    }else {
        // conversion needed, two-ways
        switch (_units) {
            case Years:
                switch (p.units()){
                    case Months:
                        // Years += Months
                        _units = Months;
                        _len = _len * 12 + p.length();
                        break;
                    case Weeks:
                    case Days:
                        // Years += Weeks/Days
                        myQL_REQUIRE(p.length()==0,
                                     "impossible addition between " << *this << " and " << p);
                        break;
                    default:
                        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
                }
                break;
            case Months:
                switch (p.units()) {
                    case Years:
                        // Months += Years
                        _len += p.length() * 12;
                        break;
                    case Weeks:
                    case Days:
                        // Months += Weeks/Days
                        myQL_REQUIRE(p.length() == 0,
                                     "impossible addition between " << *this << " and " << p);
                        break;
                    default:
                        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
                }
                break;
            case Weeks:
                switch (p.units()) {
                    case Days:
                        // Weeks += Days
                        _units = Days;
                        _len = _len * 7 + p.length();
                        break;
                    case Years:
                    case Months:
                        // Weeks += Years/Months
                        myQL_REQUIRE(p.length() == 0,
                                     "impossible addition between " << *this << " and " << p);
                        break;
                    default:
                        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
                }
                break;
            case Days:
                switch(p.units()) {
                    case Weeks:
                        // Days += Weeks
                        _len += p.length() * 7;
                        break;
                    case Years:
                    case Months:
                        // Days += Years/Months
                        myQL_REQUIRE(p.length() == 0,
                                     "impossible addition between " << *this << " and " << p);
                        break;
                    default:
                        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
                }
                break;
            default:
                myQL_FAIL("unknown time unit (" << int(_units) << ")");
        }
    }
    
    // this->normalize();
    return *this;
}

Period& Period::operator-=(const Period& p){
    return operator+=(-p);
}

Period& Period::operator/=(int n) {
    myQL_REQUIRE(n != 0, "cannot be divided by zero");
    if(_len % n == 0){
        // keep the original units
        _len /= n;
    }else{
        // try
        TimeUnit units = _units;
        int len = _len;
        switch(units){
            case Years:
                len *= 12;
                units = Months;
                break;
            case Weeks:
                len *= 7;
                units = Days;
                break;
            default:
                ;
        }
        myQL_REQUIRE(len % n == 0, *this << " cannot be divided by " << n);
        _len = len / n;
        _units = units;
    }
    return *this;
}

namespace {
std::pair<int, int> days_min_max(const Period& p){
    switch(p.units()){
        case Days:
            return std::make_pair(p.length(), p.length());
        case Weeks:
            return std::make_pair(7*p.length(), 7*p.length());
        case Months:
            return std::make_pair(28*p.length(), 31*p.length());
        case Years:
            return std::make_pair(365*p.length(), 366*p.length());
        default:
            myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}
}

double years(const Period& p){
    if(p.length() == 0) return 0.0;
    switch(p.units()){
        case Days:
            myQL_FAIL("cannot convert Days into Years");
        case Weeks:
            myQL_FAIL("cannot convert Weeks into Years");
        case Months:
            return p.length() / 12.0;
        case Years:
            return p.length();
        default:
            myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}

double months(const Period& p){
    if(p.length() == 0) return 0.0;
    switch(p.units()){
        case Days:
            myQL_FAIL("cannot convert Days into Months");
        case Weeks:
            myQL_FAIL("cannot convert Weeks into Months");
        case Months:
            return p.length();
        case Years:
            return p.length() * 12.0;
        default:
            myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}

double weeks(const Period& p) {
    if (p.length()==0) return 0.0;

    switch (p.units()) {
      case Days:
          return p.length() / 7.0;
      case Weeks:
          return p.length();
      case Months:
        myQL_FAIL("cannot convert Months into Weeks");
      case Years:
        myQL_FAIL("cannot convert Years into Weeks");
      default:
        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}

double days(const Period& p) {
    if (p.length()==0) return 0.0;

    switch (p.units()) {
      case Days:
          return p.length();
      case Weeks:
          return p.length() * 7.0;
      case Months:
        myQL_FAIL("cannot convert Months into Days");
      case Years:
        myQL_FAIL("cannot convert Years into Days");
      default:
        myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}

bool operator<(const Period& p1, const Period& p2) {
    // special cases
    if (p1.length() == 0)
        return p2.length() > 0;
    if (p2.length() == 0)
        return p1.length() < 0;
    // exact comparisons
    if (p1.units() == p2.units())
        return p1.length() < p2.length();
    if (p1.units() == Months && p2.units() == Years)
        return p1.length() < 12*p2.length();
    if (p1.units() == Years && p2.units() == Months)
        return 12*p1.length() < p2.length();
    if (p1.units() == Days && p2.units() == Weeks)
        return p1.length() < 7*p2.length();
    if (p1.units() == Weeks && p2.units() == Days)
        return 7*p1.length() < p2.length();
    // inexact comparisons (handled by converting to days and using limits)
    std::pair<int, int> p1lim = days_min_max(p1);
    std::pair<int, int> p2lim = days_min_max(p2);

    if (p1lim.second < p2lim.first)
        return true;
    else if (p1lim.first > p2lim.second)
        return false;
    else
        myQL_FAIL("undecidable comparison between " << p1 << " and " << p2);
}

Period operator+(const Period& p1, const Period& p2) {
    Period result = p1;
    result += p2;
    return result;
}

Period operator-(const Period& p1, const Period& p2) {
    return p1+(-p2);
}

Period operator/(const Period& p, int n) {
    Period result = p;
    result /= n;
    return result;
}

// period formatting
std::ostream& operator<<(std::ostream& out, const Period& p){
    int n = p.length();
    int m = 0;
    switch(p.units()){
        case Days:
            if(n>=7){
                m = n / 7;
                out << m << "W";
                n = n % 7;
            }
            if (n != 0 || m==0)
                return out << n << "D";
            else
                return out;
        case Weeks:
            return out << n << "W";
        case Months:
            if(n>=12){
                m = n/12;
                out << m << "Y";
                n = n%12;
            }
            if(n!=0 || m == 0)
                return out << n << "M";
            else
                return out;
        case Years:
            return out << n << "Y";
        default:
            myQL_FAIL("unknown time unit (" << int(p.units()) << ")");
    }
}


}

