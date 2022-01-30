//
//  registration.cpp
//  derivs
//
//  Created by Xin Li on 1/30/22.
//

#include "factory_constructible.h"

// invisible and global variables, initialization are done before main is called
namespace {
PayoffHelper<CallPayoff> RegisterCall("call");
PayoffHelper<PutPayoff> RegisterPut("put");
PayoffHelper<ForwardPayoff> RegisterForward("forward");
}





