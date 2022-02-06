//
//  factory_constructible.h
//  derivs
//
//  Created by Xin Li on 1/30/22.
//

#ifndef factory_constructible_h
#define factory_constructible_h

/*Helper class for objects that can be registered and constructed from facotry interfaces
 */

#include "factory.hpp"

template<class TBase, class TDerived>
class FactoryHelper{
public:
    FactoryHelper(std::string);
    static TBase* create(const ArgumentList&);
    ~FactoryHelper(){}
};

template<class TBase, class TDerived>
FactoryHelper<TBase, TDerived>::FactoryHelper(std::string id){
    to_lower_case(id);
    FactoryInstance<TBase>().register_class(id, FactoryHelper<TBase, TDerived>::create);
}

template<class TBase, class TDerived>
TBase* FactoryHelper<TBase, TDerived>::create(const ArgumentList& input){
    return new TDerived(input);
}




/* not a generic helper

template<typename T> // T is a concrete inherited class from base class
class PayoffHelper{
public:
    PayoffHelper(std::string payoff_id); // register payoff id in constructor
    static Payoff* create(double strike); // provide an actual function to create a concrete object if needed, method is associated to class not objects or instances
};

// register
template<typename T>
PayoffHelper<T>::PayoffHelper(std::string payoff_id){
    PayoffFactory& the_factory = PayoffFactory::instance();
    the_factory.register_payoff(payoff_id, PayoffHelper<T>::create);
}

// create
template<typename T>
Payoff* PayoffHelper<T>::create(double strike){
    return new T(strike);
}

*/

#endif /* factory_constructible_h */
