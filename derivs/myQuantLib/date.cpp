//
//  date.cpp
//  derivs
//
//  Created by Xin Li on 2/18/22.
//

#include "date.hpp"
#include "errors.hpp"

namespace myQuantLib {
// TODO: implement the rest of Date functions;
void Date::check_serial_number(Date::serial_type serial_number){
    myQL_REQUIRE(serial_number >= min_serial_number() &&
                 serial_number <= max_serial_number(),
                 "Date's serial number (" << serial_number << ") outside "
                 "allowed range [" << min_serial_number() << "-" << max_serial_number() << "], i.e. [" << min_date() << "-" << max_date() << "]"
                 );
}

// constructors
Date::Date(Date::serial_type serial_number): _serial_number(serial_number){
    check_serial_number(serial_number);
}

Date::Date(Day d, Month m, Year y){
    // check year
    myQL_REQUIRE(y > 1900 && y < 2200,
                 "year " << y << " out of bound. It must be in [1901, 2199]");
    // check month
    myQL_REQUIRE(int(m) > 0 && int(m) < 13,
                 "month " << int(m) << " outside Jan-Dec range [1, 12]");
    // check day
    bool leap = is_leap(y);
    Day len = month_len(m, leap), offset = month_offset(m, leap);
    myQL_REQUIRE(d <= len && d > 0,
                 "day outside month (" << int(m) << ") day-range "
                 << "[1," << len << "]");
    // serial_number = day + month_offset + year_offset
    _serial_number = d + offset + year_offset(y);
}



}
