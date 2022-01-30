//
//  factory.cpp
//  derivs
//
//  Created by Xin Li on 1/30/22.
//

#include "factory.hpp"
#include <iostream>

// singleton
PayoffFactory& PayoffFactory:: instance()
{
    static PayoffFactory the_factory; // in global scope
    return the_factory;
}

// register
void PayoffFactory::register_payoff(std::string payoff_id, create_payoff_func creator_func)
{
    creator_funcs.insert(std::pair<std::string, create_payoff_func>
                         (payoff_id, creator_func));
}

// manufacture
Payoff* PayoffFactory::create_payoff(std::string payoff_id, double strike)
{
    std::map<std::string, create_payoff_func>::const_iterator i = creator_funcs.find(payoff_id);
    if(i == creator_funcs.end()){
        // do not find in registered ids
        std::cout << payoff_id << " is an unknown payoff" << std::endl;
        return nullptr;
    }
    
    return (i->second)(strike);
}

// retrieve all registered names
std::vector<std::string> PayoffFactory::get_all_ids() const
{
    std::vector<std::string> ids;
    for(const auto& entry: creator_funcs) ids.push_back(entry.first);
    return ids;
}


