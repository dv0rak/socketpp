#ifndef __SOCK_EXCEPTION_H
#define __SOCK_EXCEPTION_H 1

#include <stdexcept>
#include <cerrno>
#include <string>
#include <cstring>
#include <netdb.h>

namespace socketpp {

class error : public std::runtime_error {
public:
    const char * what() const throw() { return msg.c_str(); }
    ~error() throw() { }

protected:
    std::string msg;

    error() : runtime_error("") { }
};


///@brief	socket-related exception class
class sock_error : public error {
private:
    int _errno;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	errno code
    ///@param	func	C function which returned error
    sock_error(const std::string& meth, int err, const std::string& func="") throw() : _errno(err)
    {
        msg = meth + "(): " + ::strerror(_errno);
        if(func != "")
            msg += " [C "+func+"()]";
    }

    int get_errno() const throw() { return _errno; }
};

///@brief	exception class related to C getaddrinfo() and getnameinfo() functions
class gai_error : public error {
private:
    int _gai_errno;

public:
    ///@param	meth 	method which threw exception
    ///@param	err	gai_errno code
    ///@param	func	C function which returned error
    gai_error(const std::string& meth, int err, const std::string& func="") throw() : _gai_errno(err)
    {
        msg = meth + "(): " + ::gai_strerror(_gai_errno);
        if(func != "")
            msg += " [C "+func+"()]";
    }
    
    int get_gai_errno() const throw() { return _gai_errno; }
};

///@brief	address-related exception class
class h_error : public error {
public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    h_error(const std::string& meth, const std::string& err, const std::string& func="") throw()
    {
        msg = meth + "(): " + err;
        if(func != "")
            msg += " [C "+func+"()]";
    }
};

///@brief	timeout-related exception class
class timeout : public error {
public:
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    timeout(const std::string& meth, const std::string& err) throw()
    {
        msg = meth + "(): " + err;
    }
};

};
#endif
