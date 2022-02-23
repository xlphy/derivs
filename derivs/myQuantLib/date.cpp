//
//  date.cpp
//  derivs
//
//  Created by Xin Li on 2/18/22.
//

#include "date.hpp"
#include "errors.hpp"
#include <ctime>
#include <boost/functional/hash.hpp>

namespace myQuantLib {
// _serial_number = 1 -> Jan 1st 1900, keep a total number of days starting Jan 1st 1900
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

Date::Date(const std::string& date_str) {
    // assume format yyyy-m-d
    std::string delimiter = "-";
    std::size_t beg_pos = 0, end_pos=0;
    // find year
    end_pos = date_str.find(delimiter, beg_pos);
    Year y = std::stoi(date_str.substr(beg_pos, end_pos-beg_pos));
    // find month
    beg_pos = end_pos + 1;
    end_pos = date_str.find(delimiter, beg_pos);
    Month m = Month(std::stoi(date_str.substr(beg_pos, end_pos-beg_pos)));
    // find day
    beg_pos = end_pos + 1;
    Day d = std::stoi(date_str.substr(beg_pos));
    // create an instance, checking and use its _serial_number
    Date tmp(d, m, y);
    _serial_number = tmp.serial_number();
}


// inspectors
Month Date::month() const {
    Day d = day_of_year();
    // find m that month_offset(m) < d <= month_offset(m+1)
    int m = d/30 + 1; // initial guess
    // correct for m
    bool leap = is_leap(year());
    while (d <= month_offset(Month(m), leap))
        --m;
    while (d > month_offset(Month(m+1), leap))
        ++m;
    return Month(m);
}

Year Date::year() const {
    Year y = (_serial_number / 365) + 1900;
    // if it is in current year y, then _serial_number > year_offset(y)
    if (_serial_number <= year_offset(y))
        --y;
    return y;
}

Date& Date::operator+=(Date::serial_type days){
    Date::serial_type serial = _serial_number + days;
    check_serial_number(serial);
    _serial_number = serial;
    return *this;
}

Date& Date::operator+=(const Period& p) {
    _serial_number = advance(*this, p.length(), p.units()).serial_number();
    return *this;
}

Date& Date::operator-=(Date::serial_type days){
    Date::serial_type serial = _serial_number - days;
    check_serial_number(serial);
    _serial_number = serial;
    return *this;
}

Date& Date::operator-=(const Period& p) {
    _serial_number = advance(*this, -p.length(), p.units()).serial_number();
    return *this;
}

Date& Date::operator++() {
    Date::serial_type serial = _serial_number + 1;
    check_serial_number(serial);
    _serial_number = serial;
    return *this;
}

Date Date::operator++(int ){
    Date old(*this);
    ++*this; //use the pre-increment
    return old;
}

Date& Date::operator--() {
    Date::serial_type serial = _serial_number - 1;
    check_serial_number(serial);
    _serial_number = serial;
    return *this;
}

Date Date::operator--(int ) {
    Date old(*this);
    --*this; //use the pre-increment
    return old;
}

Date Date::advance(const Date& date, int n, TimeUnit units) {
    switch (units) {
        case Days:
            return date + n;
        case Weeks:
            return date + 7 * n;
        case Months:{
            Day d = date.day_of_month();
            int m = int(date.month()) + n;
            Year y = date.year();
            while (m > 12){
                m -= 12;
                y += 1;
            }
            while (m < 1) {
                m += 12;
                y -= 1;
            }
            
            myQL_ENSURE(y >= 1900 && y <= 2199,
                        "year" << y << " out of bounds. "
                        << "It must be in [1901, 2199]");
            
            int len = month_len(Month(m), is_leap(y));
            if(d>len) d=len;
            
            return {d, Month(m), y};
        }
        case Years: {
            Day d = date.day_of_month();
            Month m = date.month();
            Year y = date.year() + n;
            
            myQL_ENSURE(y >= 1900 && y <= 2199,
                        "year " << y << " out of bounds."
                        << "It must be in [1901, 2199]");
            
            if(d == 29 && m == February && !is_leap(y))
                d = 28;
            return {d, m, y};
        }
        default:
            myQL_FAIL("undefined time units");
    }
}

Date Date::today(){
    std::time_t t;
    if(std::time(&t) == std::time_t(-1)) // -1 means time() didn't work
        return {};
    std::tm *lt = std::localtime(&t);
    // use convention from std::tm
    return {Day(lt->tm_mday), Month(lt->tm_mon + 1), Year(lt->tm_year + 1900)};
}

Date Date::next_weekday(const Date &d, Weekday w){
    // if today is Sun, next Monday is tomorrow, next Sun is today.
    Weekday wd = d.weekday();
    return d + ((wd > w ? 7 : 0) - wd + w);
}

Date Date::nth_weekday(std::size_t n, Weekday w, Month m, Year y){
    // given Year and Month, what is nth weekday, e.g., 3rd Friday in March, 2021
    myQL_REQUIRE(n > 0,
                 "zeroth day of week in a given (month, year) is undefined");
    myQL_REQUIRE(n < 6,
                 "no more than 5 weekday in a given (month, year)");
    Weekday first = Date(1, m, y).weekday();
    std::size_t skip = n - (w >= first ? 1 : 0);
    return {Day((1 + w + skip * 7) - first), m, y};
}

std::size_t hash_value(const Date& d){
    return boost::hash<Date::serial_type>()(d.serial_number());
}

// formatted output - Month
std::ostream& operator<<(std::ostream& out, Month m) {
    switch (m) {
        case January:
            return out << "January";
        case February:
            return out << "February";
        case March:
            return out << "March";
        case April:
            return out << "April";
        case May:
            return out << "May";
        case June:
            return out << "June";
        case July:
            return out << "July";
        case August:
            return out << "August";
        case September:
            return out << "September";
        case October:
            return out << "October";
        case November:
            return out << "November";
        case December:
            return out << "December";
        default:
            myQL_FAIL("unknown month (" << int(m) << ")");
    }
}

// formatted output - Date
std::ostream& operator<<(std::ostream& out, const Date& d) {
    if (d == Date()) {
        out << "null date";
    } else {
        out << d.year() << "-" << int(d.month()) << "-" << int(d.day_of_month());
    }
    return out;
}


std::ostream& operator<<(std::ostream& out, const Weekday& wd) {
    switch (wd) {
        case Sunday:
            return out << "Sun";
        case Monday:
          return out << "Mon";
        case Tuesday:
          return out << "Tue";
        case Wednesday:
          return out << "Wed";
        case Thursday:
          return out << "Thu";
        case Friday:
          return out << "Fri";
        case Saturday:
          return out << "Sat";
        default:
          myQL_FAIL("unknown weekday");
    }
}


// helper functions
void Date::check_serial_number(Date::serial_type serial_number){
    myQL_REQUIRE(serial_number >= min_serial_number() &&
                 serial_number <= max_serial_number(),
                 "Date's serial number (" << serial_number << ") outside "
                 "allowed range [" << min_serial_number() << "-" << max_serial_number() << "], i.e. [" << min_date() << "-" << max_date() << "]"
                 );
}

// compute once and store the result
bool Date::is_leap(Year y) {
    static const bool year_is_leap[] = {
        // 1900 - 1909
        false,false,false,false,true,false,false,false,true,false,
        // 1910-1919
        false,false, true,false,false,false, true,false,false,false,
        // 1920-1929
         true,false,false,false, true,false,false,false, true,false,
        // 1930-1939
        false,false, true,false,false,false, true,false,false,false,
        // 1940-1949
         true,false,false,false, true,false,false,false, true,false,
        // 1950-1959
        false,false, true,false,false,false, true,false,false,false,
        // 1960-1969
         true,false,false,false, true,false,false,false, true,false,
        // 1970-1979
        false,false, true,false,false,false, true,false,false,false,
        // 1980-1989
         true,false,false,false, true,false,false,false, true,false,
        // 1990-1999
        false,false, true,false,false,false, true,false,false,false,
        // 2000-2009
         true,false,false,false, true,false,false,false, true,false,
        // 2010-2019
        false,false, true,false,false,false, true,false,false,false,
        // 2020-2029
         true,false,false,false, true,false,false,false, true,false,
        // 2030-2039
        false,false, true,false,false,false, true,false,false,false,
        // 2040-2049
         true,false,false,false, true,false,false,false, true,false,
        // 2050-2059
        false,false, true,false,false,false, true,false,false,false,
        // 2060-2069
         true,false,false,false, true,false,false,false, true,false,
        // 2070-2079
        false,false, true,false,false,false, true,false,false,false,
        // 2080-2089
         true,false,false,false, true,false,false,false, true,false,
        // 2090-2099
        false,false, true,false,false,false, true,false,false,false,
        // 2100-2109
        false,false,false,false, true,false,false,false, true,false,
        // 2110-2119
        false,false, true,false,false,false, true,false,false,false,
        // 2120-2129
         true,false,false,false, true,false,false,false, true,false,
        // 2130-2139
        false,false, true,false,false,false, true,false,false,false,
        // 2140-2149
         true,false,false,false, true,false,false,false, true,false,
        // 2150-2159
        false,false, true,false,false,false, true,false,false,false,
        // 2160-2169
         true,false,false,false, true,false,false,false, true,false,
        // 2170-2179
        false,false, true,false,false,false, true,false,false,false,
        // 2180-2189
         true,false,false,false, true,false,false,false, true,false,
        // 2190-2199
        false,false, true,false,false,false, true,false,false,false,
        // 2200
        false
    };
    myQL_REQUIRE(y>=1900 && y<=2200, "year outside valid range");
    return year_is_leap[y-1900];
}

int Date::month_len(Month m, bool leap_year){
    static const int month_len[] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    static const int month_leap_len[] = {
        31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    return leap_year? month_leap_len[m-1] : month_len[m-1];
}

int Date::month_offset(Month m, bool leap_year){
    static const int month_offset[] = {
        0,  31,  59,  90, 120, 151, // Jan - Jun
      181, 212, 243, 273, 304, 334, // Jun - Dec
      365   // used in day_of_month to bracket day
    };
    static const int month_leap_offset[] = {
        0,  31,  60,  91, 121, 152, // Jan - Jun
      182, 213, 244, 274, 305, 335, // Jun - Dec
      366   // used in day_of_month to bracket day
    };
    return leap_year? month_leap_offset[m-1] : month_offset[m-1];
}

Date::serial_type Date::year_offset(Year y){
    // the list of all December 31st in the preceding year
    // year_offset(1901) = year_offset[1] = 365, Dec 31 1900
    static const Date::serial_type year_offset[] = {
        // 1900-1909
            0,   365,   730,  1095,  1460,  1826,  2191,  2556,  2921,  3287,
        // 1910-1919
          3652,  4017,  4382,  4748,  5113,  5478,  5843,  6209,  6574,  6939,
        // 1920-1929
          7304,  7670,  8035,  8400,  8765,  9131,  9496,  9861, 10226, 10592,
        // 1930-1939
         10957, 11322, 11687, 12053, 12418, 12783, 13148, 13514, 13879, 14244,
        // 1940-1949
         14609, 14975, 15340, 15705, 16070, 16436, 16801, 17166, 17531, 17897,
        // 1950-1959
         18262, 18627, 18992, 19358, 19723, 20088, 20453, 20819, 21184, 21549,
        // 1960-1969
         21914, 22280, 22645, 23010, 23375, 23741, 24106, 24471, 24836, 25202,
        // 1970-1979
         25567, 25932, 26297, 26663, 27028, 27393, 27758, 28124, 28489, 28854,
        // 1980-1989
         29219, 29585, 29950, 30315, 30680, 31046, 31411, 31776, 32141, 32507,
        // 1990-1999
         32872, 33237, 33602, 33968, 34333, 34698, 35063, 35429, 35794, 36159,
        // 2000-2009
         36524, 36890, 37255, 37620, 37985, 38351, 38716, 39081, 39446, 39812,
        // 2010-2019
         40177, 40542, 40907, 41273, 41638, 42003, 42368, 42734, 43099, 43464,
        // 2020-2029
         43829, 44195, 44560, 44925, 45290, 45656, 46021, 46386, 46751, 47117,
        // 2030-2039
         47482, 47847, 48212, 48578, 48943, 49308, 49673, 50039, 50404, 50769,
        // 2040-2049
         51134, 51500, 51865, 52230, 52595, 52961, 53326, 53691, 54056, 54422,
        // 2050-2059
         54787, 55152, 55517, 55883, 56248, 56613, 56978, 57344, 57709, 58074,
        // 2060-2069
         58439, 58805, 59170, 59535, 59900, 60266, 60631, 60996, 61361, 61727,
        // 2070-2079
         62092, 62457, 62822, 63188, 63553, 63918, 64283, 64649, 65014, 65379,
        // 2080-2089
         65744, 66110, 66475, 66840, 67205, 67571, 67936, 68301, 68666, 69032,
        // 2090-2099
         69397, 69762, 70127, 70493, 70858, 71223, 71588, 71954, 72319, 72684,
        // 2100-2109
         73049, 73414, 73779, 74144, 74509, 74875, 75240, 75605, 75970, 76336,
        // 2110-2119
         76701, 77066, 77431, 77797, 78162, 78527, 78892, 79258, 79623, 79988,
        // 2120-2129
         80353, 80719, 81084, 81449, 81814, 82180, 82545, 82910, 83275, 83641,
        // 2130-2139
         84006, 84371, 84736, 85102, 85467, 85832, 86197, 86563, 86928, 87293,
        // 2140-2149
         87658, 88024, 88389, 88754, 89119, 89485, 89850, 90215, 90580, 90946,
        // 2150-2159
         91311, 91676, 92041, 92407, 92772, 93137, 93502, 93868, 94233, 94598,
        // 2160-2169
         94963, 95329, 95694, 96059, 96424, 96790, 97155, 97520, 97885, 98251,
        // 2170-2179
         98616, 98981, 99346, 99712,100077,100442,100807,101173,101538,101903,
        // 2180-2189
        102268,102634,102999,103364,103729,104095,104460,104825,105190,105556,
        // 2190-2199
        105921,106286,106651,107017,107382,107747,108112,108478,108843,109208,
        // 2200
        109573,
    };
    return year_offset[y-1900];
}



}
