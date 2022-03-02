//
//  visitor.hpp
//  derivs
//
//  Created by Xin Li on 3/1/22.
//

#ifndef visitor_hpp
#define visitor_hpp

namespace myQuantLib {

// degenerate base class for the Acyclic Visitor Pattern
class AcyclicVisitor {
public:
    virtual ~AcyclicVisitor() = default;
};

// Visitor for a specific class
template <class T>
class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(T&) = 0;
};




}


#endif /* visitor_hpp */
