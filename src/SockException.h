#ifndef __SOCK_EXCEPTION_H
#define __SOCK_EXCEPTION_H 1

#include <exception>
#include <cerrno>
#include <string>
#include <cstring>

namespace socketpp {

///@brief	socket++ exception class
class SockException : public std::exception {
public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    SockException(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg=meth+"(): "+err;
        if(!func.empty()) {
            msg+=" [C "+func+"()]";
        }
        code=0;
    }
    
    ///@param	meth 	method which threw exception
    ///@param	err	errno code
    ///@param	func	C function which returned error
    SockException(const std::string& meth, int err, const std::string& func="") throw()
    {
        msg=meth+"(): "+::strerror(err);
        if(!func.empty()) {
            msg+=" [C "+func+"()]";
        }
        code=err;
    }

    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }
    ///@brief	returns errno code
    inline int getErrno() { return code; }

    ~SockException() throw() {}

private:
    std::string msg;
    int code;
};
};
#endif
