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
#include "payoff.hpp"

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

//TODO: implement a generic template factory



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


#endif /* factory_hpp */
