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
    Observable(){};
    virtual ~Observable(){}; // do not delete Observer* since Observable does not own them
    void notify_observers();
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


//! %observable and assignable proxy to concrete value
/*! Observers can be registered with instances of this class so
    that they are notified when a different value is assigned to
    such instances. Client code can copy the contained value or
    pass it to functions via implicit conversion.
    \note it is not possible to call non-const method on the
          returned value. This is by design, as this possibility
          would necessarily bypass the notification code; client
          code should modify the value via re-assignment instead.
*/

template <class T>
class ObservableValue {
public:
    // constructors
    ObservableValue(): _value(), _observable(new Observable){}
    ObservableValue(const T& t):_value(t), _observable(new Observable){}
    ObservableValue(const ObservableValue<T>& t):_value(t._value), _observable(new Observable){}
    
    // controlled assignment
    ObservableValue<T>& operator=(const T&);
    ObservableValue<T>& operator=(const ObservableValue<T>&);
    
    // implicit conversions
    operator T() const {return _value;}
    operator std::shared_ptr<Observable>() const {return _observable;} // so it can register observers
    
    const T& value() const {return _value;}
private:
    T _value;
    std::shared_ptr<Observable> _observable;
};


// template definitions
// assignments trigger notifications
template <class T>
ObservableValue<T>& ObservableValue<T>::operator=(const T& t){
    _value = t;
    _observable->notify_observers();
    return *this;
}

template <class T>
ObservableValue<T>& ObservableValue<T>::operator=(const ObservableValue<T>& t){
    _value = t._value;
    _observable->notify_observers();
    return *this;
}



}
#endif /* observer_hpp */
