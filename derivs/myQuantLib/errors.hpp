//
//  errors.hpp
//  derivs
//
//  Created by Xin Li on 2/19/22.
//

#ifndef errors_hpp
#define errors_hpp

#include <exception>
#include <string>
#include <memory>
#include <boost/current_function.hpp>
#include <sstream>

namespace myQuantLib{

class Error: public std::exception {
public:
    Error(const std::string& file,
          long line,
          const std::string& func_name,
          const std::string& message = "");
    
    const char* what() const noexcept override;
private:
    std::shared_ptr<std::string> _msg;
};

#define myQL_REQUIRE(condition, message)\
if(!(condition)){\
    std::ostringstream _msg_stream;\
    _msg_stream << message; \
    throw myQuantLib::Error(__FILE__, __LINE__, \
                            BOOST_CURRENT_FUNCTION, _msg_stream.str()); \
}\
else

#define myQL_ENSURE(condition, message)\
if(!(condition)) { \
    std::ostringstream _msg_stream; \
    _msg_stream << message; \
    throw myQuantLib::Error(__FILE__, __LINE__, \
                            BOOST_CURRENT_FUNCTION, _msg_stream.str()); \
} \
else

#define myQL_MULTILINE_FAILURE_BEGIN do {
#define myQL_MULTILINE_FAILURE_END } while(false)

#define myQL_FAIL(message)\
myQL_MULTILINE_FAILURE_BEGIN \
    std::ostringstream _msg_stream; \
    _msg_stream << message; \
    throw myQuantLib::Error(__FILE__, __LINE__, \
                            BOOST_CURRENT_FUNCTION, _msg_stream.str()); \
myQL_MULTILINE_FAILURE_END


}


#endif /* errors_hpp */
