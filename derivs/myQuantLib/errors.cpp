//
//  errors.cpp
//  derivs
//
//  Created by Xin Li on 2/19/22.
//

#include "errors.hpp"


namespace {

std::string format(const std::string& file, long line,
                   const std::string& function,
                   const std::string& message){
    std::ostringstream msg;
    msg << "\n" << file << ":" << line << ": ";
    if(function != "(unknown)")
        msg << "In function `" << function << "': \n";
    msg << message;
    return msg.str();
}

}

namespace myQuantLib {

Error::Error(const std::string& file, long line,
             const std::string& function,
             const std::string& message){
    _msg = std::make_shared<std::string>(format(file, line, function, message));
}

const char* Error::what() const noexcept {return _msg->c_str();}
}
