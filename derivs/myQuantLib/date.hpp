//
//  date.hpp
//  derivs
//
//  Created by Xin Li on 2/18/22.
//

#ifndef date_hpp
#define date_hpp

#include <iostream>
#include <functional>
#include <string>
#include "period.hpp"

namespace myQuantLib{

enum TimeUnit { Days,
                Weeks,
                Months,
                Years,
                Hours,
                Minutes,
                Seconds,
                Milliseconds,
                Microseconds
};
std::ostream& operator<<(std::ostream&, const TimeUnit&);


enum Weekday {
    Sunday    = 1,
    Monday    = 2,
    Tuesday   = 3,
    Wednesday = 4,
    Thursday  = 5,
    Friday    = 6,
    Saturday  = 7,
    Sun = 1,
    Mon = 2,
    Tue = 3,
    Wed = 4,
    Thu = 5,
    Fri = 6,
    Sat = 7
};
std::ostream& operator<<(std::ostream&, const Weekday&);

enum Month {
    January   = 1,
    February  = 2,
    March     = 3,
    April     = 4,
    May       = 5,
    June      = 6,
    July      = 7,
    August    = 8,
    September = 9,
    October   = 10,
    November  = 11,
    December  = 12,
    Jan = 1,
    Feb = 2,
    Mar = 3,
    Apr = 4,
    Jun = 6,
    Jul = 7,
    Aug = 8,
    Sep = 9,
    Oct = 10,
    Nov = 11,
    Dec = 12
};
std::ostream& operator<<(std::ostream&, Month);

typedef int Day;
typedef int Year;

class Date{
public:
    typedef int serial_type;
    Date(): _serial_number(Date::serial_type(0)){} // default
    
    explicit Date(Date::serial_type serial_number);
    Date(Day d, Month m, Year y);
    Date(const std::string& date_str);
    
    // inspectors
    Weekday weekday() const;
    Day day_of_month() const;
    // Jan 1st = 1
    Day day_of_year() const;
    Month month() const;
    Year year() const;
    Date::serial_type serial_number() const;
    
    // date arithmetic
    Date& operator+=(Date::serial_type days);
    Date& operator-=(Date::serial_type days);
    Date& operator+=(const Period&); //TODO: add Period
    Date& operator-=(const Period&); //TODO: add Period
    
    Date& operator++();
    Date operator++(int );
    Date& operator--();
    Date operator--(int );
    Date operator+(Date::serial_type days) const;
    Date operator-(Date::serial_type days) const;
    Date operator+(const Period&) const;
    Date operator-(const Period&) const;
    
    // static methods
    static Date today();
    static Date min_date(){
        static const Date d(min_serial_number());
        return d;
    }
    static Date max_date(){
        static const Date d(max_serial_number());
        return d;
    }
    static bool is_leap(Year y);
    static Date end_of_month(const Date& d);
    static bool is_end_of_month(const Date& d);
    // next given weekday following or equal to the given date
    // E.g, the Friday following Tuesday, January 15th 2002 was January 18th, 2002
    static Date next_weekday(const Date& d, Weekday w);
    // n-th given weekday in the given month and year
    // e.g., 4th Thursday of March, 1998 was March 26th, 1998
    static Date nth_weekday(std::size_t n, Weekday w, Month m, Year y);
private:
    // static members and methods, only _serial_number is instance specific
    static Date::serial_type min_serial_number(){return 366;} //Jan 1st, 1901
    static Date::serial_type max_serial_number(){return 109573;} // Dec 31st, 2199
    static void check_serial_number(Date::serial_type serial_number);
    
    Date::serial_type _serial_number;
    static Date advance(const Date& d, int units, TimeUnit);
    static int month_len(Month m, bool leap_year);
    static int month_offset(Month m, bool leap_year);
    static Date::serial_type year_offset(Year y);
};

inline Date::serial_type operator-(const Date& d1, const Date& d2){
    return d1.serial_number() - d2.serial_number();
}

// inline definitions
inline double days_between(const Date& d1, const Date& d2){
    return double(d2 - d1);
}
inline bool operator==(const Date& d1, const Date& d2){
    return d1.serial_number() == d2.serial_number();
}
inline bool operator!=(const Date& d1, const Date& d2){
    return d1.serial_number() != d2.serial_number();
}
inline bool operator<(const Date& d1, const Date& d2){
    return d1.serial_number() < d2.serial_number();
}
inline bool operator<=(const Date& d1, const Date& d2){
    return d1.serial_number() <= d2.serial_number();
}
inline bool operator>(const Date& d1, const Date& d2){
    return d1.serial_number() > d2.serial_number();
}
inline bool operator>=(const Date& d1, const Date& d2){
    return d1.serial_number() >= d2.serial_number();
}

std::size_t hash_value(const Date& d);
std::ostream& operator<<(std::ostream&, const Date&);

// inline definitions
inline Weekday Date::weekday() const{
    //int w = _serial_number % 7;
    //return Weekday(w == 0 ? 1: w);
    return Weekday(_serial_number % 7 + 1);
}

inline Day Date::day_of_month() const{
    return day_of_year() - month_offset(month(), is_leap(year()));
}

inline Day Date::day_of_year() const {
    return _serial_number - year_offset(year());
}

inline Date::serial_type Date::serial_number() const {
    return _serial_number;
}

inline Date Date::operator+(Date::serial_type days) const{
    return Date(_serial_number + days);
}

inline Date Date::operator+(const Period& p) const {
    return advance(*this, p.length(), p.units());
}

inline Date Date::operator-(Date::serial_type days) const{
    return Date(_serial_number - days);
}

inline Date Date::operator-(const Period& p) const {
    return advance(*this, -p.length(), p.units());
}

inline Date Date::end_of_month(const Date &d) {
    Month m = d.month();
    Year y = d.year();
    return {month_len(m, is_leap(y)), m, y};
}

inline bool Date::is_end_of_month(const Date &d){
    return d.day_of_month() == month_len(d.month(), is_leap(d.year()));
}


}


// template specialization in std namespace
namespace std {
    // so Date can be hashed and used by STL like set, map
template<>
struct hash<myQuantLib::Date>{
    std::size_t operator()(const myQuantLib::Date& d) const{
        return myQuantLib::hash_value(d);
    }
};
}



#endif /* date_hpp */
