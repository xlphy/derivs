//
//  daycounter_thirty360.cpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#include "daycounter_thirty360.hpp"

namespace myQuantLib {

namespace{
bool is_last_of_Feb(Day d, Month m, Year y){
    return m == 2 && d == 28 + (Date::is_leap(y) ? 1: 0);
}
}

std::shared_ptr<DayCounter::Impl>
Thirty360::implementation(Thirty360::Convention c,
                          const Date& termination_date,
                          bool is_last_period) {
    switch (c) {
      case USA:
        return std::shared_ptr<DayCounter::Impl>(new US_Impl);
      case European:
      case EurobondBasis:
        return std::shared_ptr<DayCounter::Impl>(new EU_Impl);
      case Italian:
        return std::shared_ptr<DayCounter::Impl>(new IT_Impl);
      case ISMA:
      case BondBasis:
        return std::shared_ptr<DayCounter::Impl>(new ISMA_Impl);
      case ISDA:
      case German:
        return std::shared_ptr<DayCounter::Impl>(new ISDA_Impl(termination_date, is_last_period));
      case NASD:
        return std::shared_ptr<DayCounter::Impl>(new NASD_Impl);
      default:
        myQL_FAIL("unknown 30/360 convention");
    }
}


Date::serial_type Thirty360::US_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    Month mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31 && dd1 >= 30) { dd2 = 30; }

    if (is_last_of_Feb(dd2, mm2, yy2) && is_last_of_Feb(dd1, mm1, yy1)) { dd2 = 30; }
    if (is_last_of_Feb(dd1, mm1, yy1)) { dd1 = 30; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}


Date::serial_type Thirty360::ISMA_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    Month mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31 && dd1 == 30) { dd2 = 30; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}

Date::serial_type Thirty360::EU_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    Month mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31) { dd2 = 30; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}

Date::serial_type Thirty360::IT_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    Month mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31) { dd2 = 30; }

    if (mm1 == 2 && dd1 > 27) { dd1 = 30; }
    if (mm2 == 2 && dd2 > 27) { dd2 = 30; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}

Date::serial_type Thirty360::ISDA_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    Month mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31) { dd2 = 30; }

    if (is_last_of_Feb(dd1, mm1, yy1)) { dd1 = 30; }

    bool is_termination_date =
        _termination_date == Date() ? _is_last_period : d2 == _termination_date;
    if (!is_termination_date && is_last_of_Feb(dd2, mm2, yy2)) { dd2 = 30; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}

Date::serial_type Thirty360::NASD_Impl::day_count(const Date& d1, const Date& d2) const {
    Day dd1 = d1.day_of_month(), dd2 = d2.day_of_month();
    int mm1 = d1.month(), mm2 = d2.month();
    Year yy1 = d1.year(), yy2 = d2.year();

    if (dd1 == 31) { dd1 = 30; }
    if (dd2 == 31 && dd1 >= 30) { dd2 = 30; }
    if (dd2 == 31 && dd1 < 30) { dd2 = 1; mm2++; }

    return 360*(yy2-yy1) + 30*(mm2-mm1) + (dd2-dd1);
}



}
