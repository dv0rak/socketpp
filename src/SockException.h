#ifndef __SOCK_EXCEPTION_H
#define __SOCK_EXCEPTION_H 1

#include <exception>
#include <cerrno>
#include <string>
#include <cstring>
#include <netdb.h>

namespace socketpp {

///@brief	socket-related exception class
class error : public std::exception {
private:
    std::string msg;
    int code;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    error(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg = meth + "(): " + err;
        if(func != "") {
            msg += " [C "+func+"()]";
        }
        code = 0;
    }
    
    ///@param	meth 	method which threw exception
    ///@param	err	errno code
    ///@param	func	C function which returned error
    error(const std::string& meth, int err, const std::string& func="") throw()
    {
        msg = meth + "(): " + ::strerror(err);
        if(func != "") {
            msg += " [C "+func+"()]";
        }
        code = err;
    }

    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }

    inline int get_errno() { return code; }
    
    ~error() throw() {}
};

///@brief	exception class related to C getaddrinfo() and getnameinfo() functions
class gai_error : public std::exception {
private:
    std::string msg;
    int code;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    gai_error(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg = meth + "(): " + err;
        if(func != "") {
            msg += " [C "+func+"()]";
        }
        code = 0;
    }
    
    ///@param	meth 	method which threw exception
    ///@param	err	h_errno code
    ///@param	func	C function which returned error
    gai_error(const std::string& meth, int err, const std::string& func="") throw()
    {
        msg = meth + "(): " + ::gai_strerror(err);
        if(func != "") {
            msg += " [C "+func+"()]";
        }
        code = err;
    }
    
    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }
    
    inline int get_gai_errno() { return code; }

    ~gai_error() throw() {}
};

///@brief	address-related exception class
class h_error : public std::exception {
private:
    std::string msg;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    h_error(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg = meth + "(): " + err;
        if(func != "") {
            msg += " [C "+func+"()]";
        }
    }
    
    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }
    
    ~h_error() throw() {}
};

///@brief	timeout-related exception class
class timeout : public std::exception {
private:
    std::string msg;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    timeout(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg = meth + "(): " + err;
        if(func != "") {
            msg += " [C "+func+"()]";
        }
    }
    
    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }
    
    ~timeout() throw() {}
};

};
#endif
