//
//  observer.hpp
//  derivs
//
//  Created by Xin Li on 2/16/22.
//

/*implement observer pattern*/

#ifndef observer_hpp
#define observer_hpp

#include <list>
#include <memory>

namespace myQuantLib{

// observer pattern: Observable notify all registered observers, observers registered with observables update when notified
class Observer;

class Observable{
    friend class Observer;
public:
    virtual ~Observable(){}; // do not delete Observer* since Observable does not own them
    void notify_observers();
protected:
    Observable(){};
private:
    void register_observer(Observer* o) {_observers.push_back(o);}
    void unregister_observer(Observer* o) {_observers.remove(o);}
    std::list<Observer*> _observers;
};

class Observer{
public:
    virtual ~Observer(){
        //remove registration from all Observables it subscribes to
        for(std::list<std::shared_ptr<Observable>>::iterator it = _observables.begin();
            it != _observables.end(); ++it ){
            (*it)->unregister_observer(this);
        }
    }
    void register_with(const std::shared_ptr<Observable>& o){
        o->register_observer(this); // access private methods because of friend class
        _observables.push_back(o);
    }
    void unregister_with(const std::shared_ptr<Observable>& o){
        o->unregister_observer(this); // access private methods because of friend class
        _observables.remove(o);
    }
    virtual void update()=0;
protected:
    Observer(){};
private:
    std::list<std::shared_ptr<Observable>> _observables;
};



}
#endif /* observer_hpp */
