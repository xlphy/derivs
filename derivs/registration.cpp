//
//  registration.cpp
//  derivs
//
//  Created by Xin Li on 1/30/22.
//

#include "factory_constructible.h"
#include "payoff.hpp"

// invisible and global variables, initialization are done before main is called
namespace {
FactoryHelper<Payoff, CallPayoff> RegisterCall("call");
FactoryHelper<Payoff, PutPayoff> RegisterPut("put");
FactoryHelper<Payoff, SpreadPayoff> RegisterSpread("spread");

/*
PayoffHelper<CallPayoff> RegisterCall("call");
PayoffHelper<PutPayoff> RegisterPut("put");
PayoffHelper<ForwardPayoff> RegisterForward("forward");
*/

}


