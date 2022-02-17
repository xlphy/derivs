//
//  handle.hpp
//  derivs
//
//  Created by Xin Li on 2/16/22.
//

#ifndef handle_hpp
#define handle_hpp

/*Handle, like a pointer to a pointer,
 we want to be able to change not only when the contents a pointer that points to change, but also when pointer itself changes
 */

#include "observer.hpp"


namespace myQuantLib{

//! Shared handle to an observable
/*! All copies of an instance of this class refer to the same
    observable by means of a relinkable smart pointer. When such
    pointer is relinked to another observable, the change will be
    propagated to all the copies.

    \pre Class T must inherit from Observable
*/

// Handle -> _link -> _h (std::shared_ptr<T>)
// Handle is used like an Observable to its external users, but it is also like an Observer to the resource it links to
// notify_observers() are triggered by:
// 1. changing of Observable contents, get notified by _h through Link::update()
// 2. changing of re-link to a different Observable, through derived class link_to method
template<class T>
class Handle{
public:
    explicit Handle(const std::shared_ptr<T>& h=std::shared_ptr<T>()): _link(new Link(h)){}
    const std::shared_ptr<T>& operator->() const {return _link->current_link();}
    const std::shared_ptr<T>& operator*() const {return _link->current_link();}
    bool empty() const {return _link->empty();}
    operator std::shared_ptr<Observable>() const {return _link;};  // automatic conversion, so register_with(Observable*) function will work for Handle, _link will register observers for Handle

protected:
    // internal Link class, acts like an observer to Handle's linked resources
    class Link: public Observable, public Observer {
    public:
        explicit Link(const std::shared_ptr<T>& h=std::shared_ptr<T>());
        void link_to(const std::shared_ptr<T>&);
        const std::shared_ptr<T> current_link() const {return _h;}
        bool empty() const {return _h.get() == nullptr;}
        void update() {notify_observers();} // triggered by _h->notify_observers()
    private:
        std::shared_ptr<T> _h;
    };
    std::shared_ptr<Link> _link;
};

template<class T>
class RelinkableHandle: public Handle<T>{
public:
    explicit RelinkableHandle(const std::shared_ptr<T>& h=std::shared_ptr<T>()):Handle<T>(h){}
    void link_to(const std::shared_ptr<T>& h){this->_link->link_to(h);} // base class cannot change link, expose it only for a derived class
};


// inline definitions
template<class T>
inline Handle<T>::Link::Link(const std::shared_ptr<T>& h): _h(h){link_to(_h);}


template<class T>
inline void Handle<T>::Link::link_to(const std::shared_ptr<T>& h){
    if(h != _h){
        if(_h) unregister_with(_h);
        _h = h;
        register_with(_h); // Link is an observer for _h, and _h is in Link's observables list,
        notify_observers(); // Link acted as an observable, notify its observers
    }
}







}


#endif /* handle_hpp */
