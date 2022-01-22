//
//  mjarray.hpp
//  derivs
//
//  Created by Xin Li on 1/22/22.
//

#ifndef mjarray_hpp
#define mjarray_hpp

/*Implementation for MJArray, optimized for speed and numerical operations, similar to valarray<double> but simpler provides range checks*/
#ifdef USE_VAL_ARRAY
// use valarray
#include <valarray>

typedef std::valarray<double> MJArray;

#else
// use custom MJArray
class MJArray{
public:
    explicit MJArray(unsigned long size=0);
    // copy
    MJArray(const MJArray& rhs);
    MJArray& operator=(const MJArray& rhs);
    
    // move
    MJArray(MJArray&& rhs);
    MJArray& operator=(MJArray&& rhs);
    
    // destructor
    ~MJArray(){delete[] valptr;}
    
    MJArray& operator=(const double& val); // assign equal value for each element
    // arithmetic operations
    MJArray& operator+=(const MJArray& operand);
    MJArray& operator-=(const MJArray& operand);
    MJArray& operator/=(const MJArray& operand);
    MJArray& operator*=(const MJArray& operand);
    
    MJArray& operator+=(const double& operand);
    MJArray& operator-=(const double& operand);
    MJArray& operator/=(const double& operand);
    MJArray& operator*=(const double& operand);
    
    // element-wise operation
    MJArray apply(double f(double)) const;
    
    // accessors
    inline double operator[](unsigned long i) const;
    inline double& operator[](unsigned long i);
    
    unsigned long size() const {return sz;}
    void resize(unsigned long new_size);
    
    // common functions on array
    double sum() const;
    double min() const;
    double max() const;
    
private:
    // invariance to keep: sz <= capacity && sz == endptr - valptr;
    double* valptr;
    double* endptr;
    unsigned long sz;
    unsigned long capacity;
};

inline double MJArray::operator[](unsigned long i) const
{
#ifdef RANGE_CHECKING
    if(i>=sz) throw("Index out of bounds");
#endif
    return valptr[i];
}

inline double& MJArray::operator[](unsigned long i)
{
#ifdef RANGE_CHECKING
    if(i>=sz) throw("Index out of bounds");
#endif
    return valptr[i];
}

#endif /*USE_VAL_ARRAY*/
#endif /* mjarray_hpp */
