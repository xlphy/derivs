//
//  test.cpp
//  derivs
//
//  Created by Xin Li on 2/22/22.
//

#include "../myQuantLib/date.hpp"

void test_date(){
    std::cout << "max date: " << myQuantLib::Date::max_date() << std::endl;
    std::cout << "min date: " << myQuantLib::Date::min_date() << std::endl;
    std::cout << "today: " << myQuantLib::Date::today() << std::endl;
    myQuantLib::Date d(myQuantLib::Day(1), myQuantLib::Month(1), myQuantLib::Year(2009));
    myQuantLib::Date d2("2009-1-1");
    std::cout << d << ", " << d2 << std::endl;
    std::cout << "Thanksgiving dates from 2009 - 2022: \n";
    myQuantLib::Year y = 2009;
    while (y <= 2022){
        std::cout << y << ": " << myQuantLib::Date::nth_weekday(4, myQuantLib::Thursday, myQuantLib::November, y) << std::endl;
        y++;
    }
    
}
