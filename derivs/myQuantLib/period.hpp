//
//  period.hpp
//  derivs
//
//  Created by Xin Li on 2/22/22.
//

#ifndef period_hpp
#define period_hpp

#include <iostream>


namespace myQuantLib {

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


enum Frequency { NoFrequency = -1,     //!< null frequency
                 Once = 0,             //!< only once, e.g., a zero-coupon
                 Annual = 1,           //!< once a year
                 Semiannual = 2,       //!< twice a year
                 EveryFourthMonth = 3, //!< every fourth month
                 Quarterly = 4,        //!< every third month
                 Bimonthly = 6,        //!< every second month
                 Monthly = 12,         //!< once a month
                 EveryFourthWeek = 13, //!< every fourth week
                 Biweekly = 26,        //!< every second week
                 Weekly = 52,          //!< once a week
                 Daily = 365,          //!< once a day
                 OtherFrequency = 999  //!< some other unknown frequency
};

/*! \relates Frequency */
std::ostream& operator<<(std::ostream& out, Frequency f);



class Period {
public:
    // constructors
    Period() = default;
    Period(int n, TimeUnit units): _len(n), _units(units){}
    explicit Period(Frequency f);
    
    int length() const {return _len;}
    TimeUnit units() const {return _units;}
    Frequency frequency() const;
    
    Period& operator+=(const Period&);
    Period& operator-=(const Period&);
    Period& operator/=(int);
    void normalize();
    
private:
    int _len = 0;
    TimeUnit _units = Days;
    
};

double years(const Period&);
double months(const Period&);
double weeks(const Period&);
double days(const Period&);

template <typename T>
Period operator*(T n, TimeUnit units);

template <typename T>
Period operator*(TimeUnit units, T n);

Period operator-(const Period&);
Period operator*(int n, const Period&);

Period operator*(const Period&, int n);
Period operator/(const Period&, int n);

Period operator+(const Period&, const Period&);
Period operator-(const Period&, const Period&);

bool operator<(const Period&, const Period&); // only need to implement this one carefully
bool operator==(const Period&, const Period&);
bool operator!=(const Period&, const Period&);
bool operator>(const Period&, const Period&);
bool operator<=(const Period&, const Period&);
bool operator>=(const Period&, const Period&);

/*! \relates Period */
std::ostream& operator<<(std::ostream&, const Period&);


template <typename T>
inline Period operator*(T n, TimeUnit units) {
    return {Integer(n), units};
}

template <typename T>
inline Period operator*(TimeUnit units, T n) {
    return {Integer(n), units};
}

inline Period operator-(const Period& p) { return {-p.length(), p.units()}; }

inline Period operator*(int n, const Period& p) { return {n * p.length(), p.units()}; }

inline Period operator*(const Period& p, int n) { return {n * p.length(), p.units()}; }

// complete comparison, only need to implement <
inline bool operator==(const Period& p1, const Period& p2) {
    return !(p1 < p2 || p2 < p1);
}

inline bool operator!=(const Period& p1, const Period& p2) {
    return !(p1 == p2);
}

inline bool operator>(const Period& p1, const Period& p2) {
    return p2 < p1;
}

inline bool operator<=(const Period& p1, const Period& p2) {
    return !(p1 > p2);
}

inline bool operator>=(const Period& p1, const Period& p2) {
    return !(p1 < p2);
}




}


#endif /* period_hpp */
