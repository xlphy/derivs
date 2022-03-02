//
//  comparison.hpp
//  derivs
//
//  Created by Xin Li on 3/2/22.
//

#ifndef comparison_hpp
#define comparison_hpp

#include <memory>

namespace myQuantLib {

//! compare two objects by date
/*! There is no generic implementation of this struct.
    Template specializations will have to be defined for
    each needed type (see CashFlow for an example.)
*/
template <class T> struct earlier_than;

/* partial specialization for shared pointers, forwarding to their
   pointees. */
template <class T>
struct earlier_than<std::shared_ptr<T> > {
    typedef std::shared_ptr<T> first_argument_type;
    typedef std::shared_ptr<T> second_argument_type;
    typedef bool result_type;

    bool operator()(const std::shared_ptr<T>& x,
                    const std::shared_ptr<T>& y) const {
        return earlier_than<T>()(*x,*y);
    }
};


}






#endif /* comparison_hpp */
