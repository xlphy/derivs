//
//  quote.hpp
//  derivs
//
//  Created by Xin Li on 3/13/22.
//

#ifndef quote_hpp
#define quote_hpp

#include "observer.hpp"

namespace myQuantLib {

class Quote: public virtual Observable {
public:
    ~Quote() override = default;
    virtual double value() const = 0;
    virtual bool is_valid() const = 0;
};
}


#endif /* quote_hpp */
