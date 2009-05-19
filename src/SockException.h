#ifndef __SOCK_EXCEPTION_H
#define __SOCK_EXCEPTION_H 1

#include <exception>
#include <cerrno>
#include <string>
#include <cstring>
#include <netdb.h>

namespace socketpp {

///@brief	base exception class
class base_error : public std::exception {
protected:
    std::string msg;
    int code;
    
    ///@param	meth 	method which threw exception
    ///@param	err	error description string
    ///@param	func	C function which returned error
    base_error(const std::string& meth, const std::string& err, const std::string& func="") throw()
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
    base_error(const std::string& meth, int err, const std::string& func="") throw()
    {
        msg = meth + "(): " + _strerror(err);
        if(func != "") {
            msg += " [C "+func+"()]";
        }
        code = err;
    }

    virtual std::string _strerror(int err)
    {
        return "";
    }

public:
    ///@brief	returns complete error string
    virtual const char * what() const throw() { return msg.c_str(); }

    ~base_error() throw() {}
};


class h_error : public base_error {
public:
    h_error(const std::string& meth, const std::string& err, const std::string& func="") throw() 
        : base_error(meth, err, func) {}
    
    h_error(const std::string& meth, int err, const std::string& func="") throw()
        : base_error(meth, err, func) {}

    int get_h_errno()
    {
        return code;
    }
    
    ~h_error() throw() {}

private:
    std::string _strerror(int err)
    {
        return ::hstrerror(err);
    }
};


class error : public base_error {
public:
    error(const std::string& meth, const std::string& err, const std::string& func="") throw() 
        : base_error(meth, err, func) {}
    
    error(const std::string& meth, int err, const std::string& func="") throw()
        : base_error(meth, err, func) {}

    int get_errno()
    {
        return code;
    }
    
    ~error() throw() {}

private:
    std::string _strerror(int err)
    {
        return ::strerror(err);
    }
};


class timeout : public base_error {
public:
    timeout(const std::string& meth, const std::string& err, const std::string& func="") throw() 
        : base_error(meth, err, func) {}
    
    ~timeout() throw() {}
};

};
#endif
