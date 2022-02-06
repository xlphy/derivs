//
//  factory.hpp
//  derivs
//
//  Created by Xin Li on 1/30/22.
//

#ifndef factory_hpp
#define factory_hpp

/*factory is a global (singleton) class which allows
 1) register classes, so the program knows what different classes are available and match the correct type (class) given an identifier (usually string) for the registered class
 2) create or manufacture a concrete class object on demand, given the identifier and constructor arguments
 */

#include <map>
#include <vector>
#include <string>

#include "arglist.hpp"

class noncopyable{
protected:
    noncopyable() = default;
    ~noncopyable() = default;
    
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
//private:
//    noncopyable(const noncopyable&);
//    noncopyable& operator=(const noncopyable&);
};

//A generic template factory
template<class T>
class ArgListFactory;

template<class T>
ArgListFactory<T>& FactoryInstance(){
    static ArgListFactory<T> obj;
    return obj;
}

//T should be a base class
template<typename T>
class ArgListFactory{
public:
    friend ArgListFactory<T>& FactoryInstance<>();
    
    typedef T* (*create_T_func)(const ArgumentList&);
    void register_class(std::string class_id, create_T_func);
    T* create_T(ArgumentList args);
    ~ArgListFactory(){};
    
    std::string get_known_types() const {return known_types;}
private:
    std::map<std::string, create_T_func> creator_funcs;
    std::string known_types;
    // avoid create ArgListFactory by itself, created only by FactoryInstance function
    ArgListFactory(){}
    ArgListFactory(const ArgListFactory&){}
    ArgListFactory& operator=(const ArgListFactory&){return *this;} // return the only copy
};

template<typename T>
void ArgListFactory<T>::register_class(std::string class_id, create_T_func creator_func){
    to_lower_case(class_id);
    creator_funcs.insert(std::pair<std::string, create_T_func>(class_id, creator_func));
    known_types += " " + class_id;
}

template<typename T>
T* ArgListFactory<T>::create_T(ArgumentList args){
    std::string id = args.get_str_arg_val("name");
    if(creator_funcs.find(id) == creator_funcs.end())
        throw(id + " is an unknown class, Known types are : " + known_types);
    return (creator_funcs.find(id)->second)(args);
}

// easy access function
template<class T>
T* get_from_factory(const ArgumentList& args){
    return FactoryInstance<T>().create_T(args);
}



/* not a generic factory
 
#include "payoff.hpp"  // note circular imports

class PayoffFactory{
public:
    typedef Payoff* (*create_payoff_func)(double );
    
    static PayoffFactory& instance(); // singleton constructor needs to be static
    
    void register_payoff(std::string, create_payoff_func); // associate id with creator pointer
    Payoff* create_payoff(std::string payoff_id, double strike); // create an object by id and strike
    
    std::vector<std::string> get_all_ids() const;
    
    ~PayoffFactory(){};
    
    
private:
    std::map<std::string, create_payoff_func> creator_funcs; // register list
    // avoid copy and creation by external users
    PayoffFactory(){}  // default constructor
    PayoffFactory(const PayoffFactory&){}  // copy constructor
    PayoffFactory& operator=(const PayoffFactory&){return *this;}  // copy assignment
};

*/

#endif /* factory_hpp */
