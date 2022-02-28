//
//  daycounter_biz252.cpp
//  derivs
//
//  Created by Xin Li on 2/27/22.
//

#include "daycounter_biz252.hpp"
#include <map>
#include <ostream>

namespace myQuantLib {

namespace {

typedef std::map<Year, std::map<Month, Date::serial_type>> Cache;
typedef std::map<Year, Date::serial_type> OuterCache;

std::map<std::string, Cache> _monthly_figures;
std::map<std::string, OuterCache> _yearly_figures;

bool is_same_year(const Date& d1, const Date& d2) {
    return d1.year() == d2.year();
}

bool is_same_month(const Date& d1, const Date& d2) {
    return d1.year() == d2.year() && d1.month() == d2.month();
}

// biz days in a given Month of Year
Date::serial_type biz_days(Cache& cache, const Calendar& calendar, Month month, Year year){
    if (cache[year][month] == 0){
        // calculate and store.
        Date d1 = Date(1, month, year);
        Date d2 = d1 + 1 * Months;
        cache[year][month] = calendar.bizdays_between(d1, d2);
    }
    return cache[year][month];
}

// biz days in a given Year
Date::serial_type biz_days(OuterCache& outer_cache, Cache& cache, const Calendar& calendar, Year year) {
    if(outer_cache[year] == 0){
        // calculate and store
        Date::serial_type total = 0;
        for(int i = 1; i <= 12; ++i){
            total += biz_days(cache, calendar, Month(i), year);
        }
        outer_cache[year] = total;
    }
    return outer_cache[year];
}

}


// implement override methods
std::string Biz252::Impl::name() const {
    std::ostringstream out;
    out << "Business/252(" << _calendar.name() << ")";
    return out.str();
}

Date::serial_type Biz252::Impl::day_count(const Date& d1, const Date& d2) const {
    if(is_same_month(d1, d2) || d1 >= d2){
        // we treat the case of d1 > d2 here, since we'd need a
        // second cache to get it right (our cached figures are
        // for first included, last excluded and might have to be
        // changed going the other way.)
        return _calendar.bizdays_between(d1, d2);
    } else if (is_same_year(d1, d2)){
        Cache& cache = _monthly_figures[_calendar.name()];
        Date::serial_type total = 0;
        Date d;
        // first, we get the beginning of next month.
        d = Date(1, d1.month(), d1.year()) + 1*Months;
        total += _calendar.bizdays_between(d1, d);
        // then, we add any whole months (whose figures might be cached already) in the middle of our period
        while(is_same_month(d, d2)) {
            total += biz_days(cache, _calendar, d.month(), d.year());
            d += 1*Months;
        }
        // finally, we get to the end of the period
        total += _calendar.bizdays_between(d, d2);
        return total;
    } else {
        Cache& cache = _monthly_figures[_calendar.name()];
        OuterCache& outer_cache = _yearly_figures[_calendar.name()];
        Date::serial_type total = 0;
        Date d;
        // first, we get to the beginning of next year.
        // the first bit gets us the end of this month ...
        d = Date(1, d1.month(), d1.year()) + 1*Months;
        total += _calendar.bizdays_between(d1, d);
        // ... then we add any remaining months, possibly cached
        for(int m = int(d1.month()) + 1; m <= 12; ++m){
            total += biz_days(cache, _calendar, Month(m), d.year());
        }
        // then, we add any whole year (might be cached) in the middle of our period
        d = Date(1, January, d1.year() + 1);
        while(!is_same_year(d, d2)) {
            total += biz_days(outer_cache, cache, _calendar, d.year());
            d += 1*Years;
        }
        // finally, we get to the end of the period.
        // First, we add whole months ...
        for(int m = 1; m < int(d2.month()); ++m){
            total += biz_days(cache, _calendar, Month(m), d2.year());
        }
        // .. then the last bit
        d = Date(1, d2.month(), d2.year());
        total += _calendar.bizdays_between(d, d2);
        return total;
    }
}

double Biz252::Impl::year_fraction(const Date &d1, const Date &d2, const Date &, const Date &) const {
    return day_count(d1, d2) / 252.0;
}

}
