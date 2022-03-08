//
//  extrapolation.hpp
//  derivs
//
//  Created by Xin Li on 3/7/22.
//

#ifndef extrapolation_hpp
#define extrapolation_hpp

namespace myQuantLib {

// base class for classes possibly allowing extrapolation
class Extrapolator {
public:
    Extrapolator() = default;
    virtual ~Extrapolator() = default;
    
    void enable_extrapolation(bool b = true) {_extrapolate = b;}
    void disable_extrapolation(bool b = true) {_extrapolate = !b;}
    
    bool allow_extrapolation() const {return _extrapolate;}
private:
    bool _extrapolate = false;
};




}


#endif /* extrapolation_hpp */
