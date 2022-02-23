//
//  period.hpp
//  derivs
//
//  Created by Xin Li on 2/22/22.
//

#ifndef period_hpp
#define period_hpp

#include "date.hpp"

namespace myQuantLib {

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



}


#endif /* period_hpp */
