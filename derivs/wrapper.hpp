//
//  wrapper.hpp
//  derivs
//
//  Created by Xin Li on 1/20/22.
//

#ifndef wrapper_hpp
#define wrapper_hpp

/*using template solution(instead of bridge pattern) to solve memory handling issues of pointers to base class
 Wrapper acts like a pointer to an object, but with added responsibilities. It also owns the object it points to.
 It should handle construct/copy/move/destruct the object it points to properly.
 The Wrapper is around base clase which allows polymorphism, it also requires clone method
 */

template<typename T>
class Wrapper{
public:
    Wrapper():ptr(nullptr){}
    Wrapper(const T& inner){ptr = inner.clone();} // convert T to wrapper, bridge func(wrapper) and func(T)
    //copy
    Wrapper(const Wrapper<T>& rhs){
        if(rhs.ptr != nullptr)
            ptr = rhs.ptr->clone();
        else
            ptr = nullptr;
    }
    Wrapper& operator=(const Wrapper<T>& rhs){
        if(this != &rhs){
            delete ptr;
            ptr = rhs.ptr != nullptr ? rhs.ptr->clone() : nullptr;
        }
        return *this;
    }
    //move
    Wrapper(Wrapper<T>&& rhs){
        ptr = rhs.ptr;
        rhs.ptr=nullptr;
    }
    Wrapper& operator=(Wrapper<T>&& rhs){
        delete ptr;
        ptr = rhs.ptr;
        rhs.ptr = nullptr;
        return *this;
    }
    //destructor
    ~Wrapper(){
        if(ptr != nullptr)
            delete ptr;
    }
    //pointer operators to access underlying data and its members/methods
    T& operator*(){return *ptr;}
    const T& operator*() const {return *ptr;}
    
    const T* operator->() const {return ptr;}
    T* operator->() {return ptr;}
    
private:
    T* ptr;
};


#endif /* wrapper_hpp */
