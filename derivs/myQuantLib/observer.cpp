//
//  observer.cpp
//  derivs
//
//  Created by Xin Li on 2/16/22.
//

#include "observer.hpp"

namespace myQuantLib {

void Observable::notify_observers(){
    for(std::list<Observer*>::iterator it = _observers.begin(); it != _observers.end(); ++it)
        (*it)->update();
}

}

