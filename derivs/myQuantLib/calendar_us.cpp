//
//  calendar_us.cpp
//  derivs
//
//  Created by Xin Li on 2/24/22.
//

#include "calendar_us.hpp"

namespace myQuantLib {
namespace {
    // a few rules used by multiple calendars

bool isWashingtonBirthday(Day d, Month m, Year y, Weekday w){
    if(y >= 1971){
        // third Monday in February
        return (d >= 15 && d <= 21) && w == Monday && m == February;
    } else {
        // February 22nd, possibly adjusted
        return (d == 22 || (d == 23 && w == Monday)
                || (d == 21 && w == Friday)) && m == February;
    }
}

bool isMemorialDay(Day d, Month m, Year y, Weekday w) {
    if (y >= 1971) {
        // last Monday in May
        return d >= 25 && w == Monday && m == May;
    } else {
        // May 30th, possibly adjusted
        return (d == 30 || (d == 31 && w == Monday)
                || (d == 29 && w == Friday)) && m == May;
    }
}

bool isLaborDay(Day d, Month m, Year y, Weekday w) {
    // first Monday in September
    return d <= 7 && w == Monday && m == September;
}

bool isColumbusDay(Day d, Month m, Year y, Weekday w) {
    // second Monday in October
    return (d >= 8 && d <= 14) && w == Monday && m == October
        && y >= 1971;
}

bool isVeteransDay(Day d, Month m, Year y, Weekday w) {
    if (y <= 1970 || y >= 1978) {
        // November 11th, adjusted
        return (d == 11 || (d == 12 && w == Monday) ||
                (d == 10 && w == Friday)) && m == November;
    } else {
        // fourth Monday in October
        return (d >= 22 && d <= 28) && w == Monday && m == October;
    }
}

bool isVeteransDayNoSaturday(Day d, Month m, Year y, Weekday w) {
    if (y <= 1970 || y >= 1978) {
        // November 11th, adjusted, but no Saturday to Friday
        return (d == 11 || (d == 12 && w == Monday)) && m == November;
    } else {
        // fourth Monday in October
        return (d >= 22 && d <= 28) && w == Monday && m == October;
    }
}

bool isJuneteenth(Day d, Month m, Year y, Weekday w) {
    // declared in 2021, but only observed by exchanges since 2022
    return (d == 19 || (d == 20 && w == Monday) || (d == 18 && w == Friday))
        && m == June && y >= 2022;
}
}

UnitedStates::UnitedStates(UnitedStates::Market market) {
    // all calendar instances on the same market share the same
    // implementation instance
    static std::shared_ptr<Calendar::Impl> settlementImpl(
                                    new UnitedStates::SettlementImpl);
    static std::shared_ptr<Calendar::Impl> liborImpactImpl(
                                    new UnitedStates::LiborImpactImpl);
    static std::shared_ptr<Calendar::Impl> nyseImpl(
                                    new UnitedStates::NyseImpl);
    static std::shared_ptr<Calendar::Impl> governmentImpl(
                                    new UnitedStates::GovernmentBondImpl);
    static std::shared_ptr<Calendar::Impl> nercImpl(
                                    new UnitedStates::NercImpl);
    static std::shared_ptr<Calendar::Impl> federalReserveImpl(
                                    new UnitedStates::FederalReserveImpl);
    switch (market) {
      case Settlement:
        _impl = settlementImpl;
        break;
      case LiborImpact:
        _impl = liborImpactImpl;
        break;
      case NYSE:
        _impl = nyseImpl;
        break;
      case GovernmentBond:
        _impl = governmentImpl;
        break;
      case NERC:
        _impl = nercImpl;
        break;
      case FederalReserve:
        _impl = federalReserveImpl;
        break;
      default:
        myQL_FAIL("unknown market");
    }
}

bool UnitedStates::SettlementImpl::is_biz_day(const Date& date) const {
    Weekday w = date.weekday();
    Day d = date.day_of_month();
    Month m = date.month();
    Year y = date.year();
    if (is_weekend(w)
        // New Year's Day (possibly moved to Monday if on Sunday)
        || ((d == 1 || (d == 2 && w == Monday)) && m == January)
        // (or to Friday if on Saturday)
        || (d == 31 && w == Friday && m == December)
        // Martin Luther King's birthday (third Monday in January)
        || ((d >= 15 && d <= 21) && w == Monday && m == January
            && y >= 1983)
        // Washington's birthday (third Monday in February)
        || isWashingtonBirthday(d, m, y, w)
        // Memorial Day (last Monday in May)
        || isMemorialDay(d, m, y, w)
        // Juneteenth (Monday if Sunday or Friday if Saturday)
        || isJuneteenth(d, m, y, w)
        // Independence Day (Monday if Sunday or Friday if Saturday)
        || ((d == 4 || (d == 5 && w == Monday) ||
             (d == 3 && w == Friday)) && m == July)
        // Labor Day (first Monday in September)
        || isLaborDay(d, m, y, w)
        // Columbus Day (second Monday in October)
        || isColumbusDay(d, m, y, w)
        // Veteran's Day (Monday if Sunday or Friday if Saturday)
        || isVeteransDay(d, m, y, w)
        // Thanksgiving Day (fourth Thursday in November)
        || ((d >= 22 && d <= 28) && w == Thursday && m == November)
        // Christmas (Monday if Sunday or Friday if Saturday)
        || ((d == 25 || (d == 26 && w == Monday) ||
             (d == 24 && w == Friday)) && m == December))
        return false; // NOLINT(readability-simplify-boolean-expr)
    return true;
}

bool UnitedStates::LiborImpactImpl::is_biz_day(const Date& date) const {
    // Since 2015 Independence Day only impacts Libor if it falls
    // on a weekday
    Weekday w = date.weekday();
    Day d = date.day_of_month();
    Month m = date.month();
    Year y = date.year();
    if (((d == 5 && w == Monday) ||
        (d == 3 && w == Friday)) && m == July && y >= 2015)
        return true;
    return SettlementImpl::is_biz_day(date);
}

bool UnitedStates::NyseImpl::is_biz_day(const Date& date) const {
    Weekday w = date.weekday();
    Day d = date.day_of_month(), dd = date.day_of_year();
    Month m = date.month();
    Year y = date.year();
    Day em = easter_monday(y);
    if (is_weekend(w)
        // New Year's Day (possibly moved to Monday if on Sunday)
        || ((d == 1 || (d == 2 && w == Monday)) && m == January)
        // Washington's birthday (third Monday in February)
        || isWashingtonBirthday(d, m, y, w)
        // Good Friday
        || (dd == em-3)
        // Memorial Day (last Monday in May)
        || isMemorialDay(d, m, y, w)
        // Juneteenth (Monday if Sunday or Friday if Saturday)
        || isJuneteenth(d, m, y, w)
        // Independence Day (Monday if Sunday or Friday if Saturday)
        || ((d == 4 || (d == 5 && w == Monday) ||
             (d == 3 && w == Friday)) && m == July)
        // Labor Day (first Monday in September)
        || isLaborDay(d, m, y, w)
        // Thanksgiving Day (fourth Thursday in November)
        || ((d >= 22 && d <= 28) && w == Thursday && m == November)
        // Christmas (Monday if Sunday or Friday if Saturday)
        || ((d == 25 || (d == 26 && w == Monday) ||
             (d == 24 && w == Friday)) && m == December)
        ) return false;

    if (y >= 1998 && (d >= 15 && d <= 21) && w == Monday && m == January)
        // Martin Luther King's birthday (third Monday in January)
        return false;

    if ((y <= 1968 || (y <= 1980 && y % 4 == 0)) && m == November
        && d <= 7 && w == Tuesday)
        // Presidential election days
        return false;

    // Special closings
    if (// President Bush's Funeral
        (y == 2018 && m == December && d == 5)
        // Hurricane Sandy
        || (y == 2012 && m == October && (d == 29 || d == 30))
        // President Ford's funeral
        || (y == 2007 && m == January && d == 2)
        // President Reagan's funeral
        || (y == 2004 && m == June && d == 11)
        // September 11-14, 2001
        || (y == 2001 && m == September && (11 <= d && d <= 14))
        // President Nixon's funeral
        || (y == 1994 && m == April && d == 27)
        // Hurricane Gloria
        || (y == 1985 && m == September && d == 27)
        // 1977 Blackout
        || (y == 1977 && m == July && d == 14)
        // Funeral of former President Lyndon B. Johnson.
        || (y == 1973 && m == January && d == 25)
        // Funeral of former President Harry S. Truman
        || (y == 1972 && m == December && d == 28)
        // National Day of Participation for the lunar exploration.
        || (y == 1969 && m == July && d == 21)
        // Funeral of former President Eisenhower.
        || (y == 1969 && m == March && d == 31)
        // Closed all day - heavy snow.
        || (y == 1969 && m == February && d == 10)
        // Day after Independence Day.
        || (y == 1968 && m == July && d == 5)
        // June 12-Dec. 31, 1968
        // Four day week (closed on Wednesdays) - Paperwork Crisis
        || (y == 1968 && dd >= 163 && w == Wednesday)
        // Day of mourning for Martin Luther King Jr.
        || (y == 1968 && m == April && d == 9)
        // Funeral of President Kennedy
        || (y == 1963 && m == November && d == 25)
        // Day before Decoration Day
        || (y == 1961 && m == May && d == 29)
        // Day after Christmas
        || (y == 1958 && m == December && d == 26)
        // Christmas Eve
        || ((y == 1954 || y == 1956 || y == 1965)
            && m == December && d == 24)
        ) return false;

    return true;
}


bool UnitedStates::GovernmentBondImpl::is_biz_day(const Date& date) const {
    Weekday w = date.weekday();
    Day d = date.day_of_month(), dd = date.day_of_year();
    Month m = date.month();
    Year y = date.year();
    Day em = easter_monday(y);
    if (is_weekend(w)
        // New Year's Day (possibly moved to Monday if on Sunday)
        || ((d == 1 || (d == 2 && w == Monday)) && m == January)
        // Martin Luther King's birthday (third Monday in January)
        || ((d >= 15 && d <= 21) && w == Monday && m == January
            && y >= 1983)
        // Washington's birthday (third Monday in February)
        || isWashingtonBirthday(d, m, y, w)
        // Good Friday (2015 was half day due to NFP report)
        || (dd == em-3 && y != 2015)
        // Memorial Day (last Monday in May)
        || isMemorialDay(d, m, y, w)
        // Juneteenth (Monday if Sunday or Friday if Saturday)
        || isJuneteenth(d, m, y, w)
        // Independence Day (Monday if Sunday or Friday if Saturday)
        || ((d == 4 || (d == 5 && w == Monday) ||
             (d == 3 && w == Friday)) && m == July)
        // Labor Day (first Monday in September)
        || isLaborDay(d, m, y, w)
        // Columbus Day (second Monday in October)
        || isColumbusDay(d, m, y, w)
        // Veteran's Day (Monday if Sunday)
        || isVeteransDayNoSaturday(d, m, y, w)
        // Thanksgiving Day (fourth Thursday in November)
        || ((d >= 22 && d <= 28) && w == Thursday && m == November)
        // Christmas (Monday if Sunday or Friday if Saturday)
        || ((d == 25 || (d == 26 && w == Monday) ||
             (d == 24 && w == Friday)) && m == December))
        return false;
         
    // Special closings
    if (// President Bush's Funeral
        (y == 2018 && m == December && d == 5)
        // Hurricane Sandy
        || (y == 2012 && m == October && (d == 30))
        // President Reagan's funeral
        || (y == 2004 && m == June && d == 11)
        ) return false;
 
    return true;
}


bool UnitedStates::NercImpl::is_biz_day(const Date& date) const {
    Weekday w = date.weekday();
    Day d = date.day_of_month();
    Month m = date.month();
    Year y = date.year();
    if (is_weekend(w)
        // New Year's Day (possibly moved to Monday if on Sunday)
        || ((d == 1 || (d == 2 && w == Monday)) && m == January)
        // Memorial Day (last Monday in May)
        || isMemorialDay(d, m, y, w)
        // Independence Day (Monday if Sunday)
        || ((d == 4 || (d == 5 && w == Monday)) && m == July)
        // Labor Day (first Monday in September)
        || isLaborDay(d, m, y, w)
        // Thanksgiving Day (fourth Thursday in November)
        || ((d >= 22 && d <= 28) && w == Thursday && m == November)
        // Christmas (Monday if Sunday)
        || ((d == 25 || (d == 26 && w == Monday)) && m == December))
        return false; // NOLINT(readability-simplify-boolean-expr)
    return true;
}


bool UnitedStates::FederalReserveImpl::is_biz_day(const Date& date) const {
    // see https://www.frbservices.org/holidayschedules/ for details
    Weekday w = date.weekday();
    Day d = date.day_of_month();
    Month m = date.month();
    Year y = date.year();
    if (is_weekend(w)
        // New Year's Day (possibly moved to Monday if on Sunday)
        || ((d == 1 || (d == 2 && w == Monday)) && m == January)
        // Martin Luther King's birthday (third Monday in January)
        || ((d >= 15 && d <= 21) && w == Monday && m == January
            && y >= 1983)
        // Washington's birthday (third Monday in February)
        || isWashingtonBirthday(d, m, y, w)
        // Memorial Day (last Monday in May)
        || isMemorialDay(d, m, y, w)
        // Juneteenth (Monday if Sunday or Friday if Saturday)
        || isJuneteenth(d, m, y, w)
        // Independence Day (Monday if Sunday)
        || ((d == 4 || (d == 5 && w == Monday)) && m == July)
        // Labor Day (first Monday in September)
        || isLaborDay(d, m, y, w)
        // Columbus Day (second Monday in October)
        || isColumbusDay(d, m, y, w)
        // Veteran's Day (Monday if Sunday)
        || isVeteransDayNoSaturday(d, m, y, w)
        // Thanksgiving Day (fourth Thursday in November)
        || ((d >= 22 && d <= 28) && w == Thursday && m == November)
        // Christmas (Monday if Sunday)
        || ((d == 25 || (d == 26 && w == Monday)) && m == December))
        return false; // NOLINT(readability-simplify-boolean-expr)
    return true;
}




}
