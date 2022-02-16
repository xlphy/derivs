//
//  instrument.cpp
//  derivs
//
//  Created by Xin Li on 2/14/22.
//

#include "instrument.hpp"


namespace myQuantLib {

void Observable::notify_observers(){
    for(std::list<Observer*>::iterator it = _observers.begin(); it != _observers.end(); ++it)
        (*it)->update();
}

}
