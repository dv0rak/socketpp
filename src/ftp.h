#ifndef __FTP_H
#define __FTP_H 1

#include "Socket.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <sstream>

namespace ftplib {

class error : public std::exception {
public:
    virtual const char * what() const throw() { return _msg.c_str(); }
    ~error() throw() {}
protected:
    std::string _msg;

    error(const std::string& msg) throw() { _msg = msg; }
};

class error_temp : public error {
public:
    error_temp(int code, const std::string &msg) throw() : error(msg) { _code = code; }
    ~error_temp() throw() {}
private:
    int _code;
};

class error_perm : public error {
public:
    error_perm(int code, const std::string &msg) throw() : error(msg) { _code = code; }
    ~error_perm() throw() {}
private:
    int _code;
};

class error_proto : public error {
public:
    error_proto(const std::string &msg) throw() : error(msg) {}
    ~error_proto() throw() {}
};

///@brief This class tries to imitate the ftplib.FTP python class, with a few differences. Before I write the whole documentation try to adapt yourself to these differences intuitively. See the python documentation.
class FTP {
public:
    FTP(const std::string &host="", const std::string &user="", const std::string &passwd="", double timeout=0.0);

    std::string connect(const std::string &host, socketpp::port_t port=21);
    std::string login(const std::string &user="anonymous", const std::string &passwd="");
    std::string sendcmd(const std::string &cmd);

    std::string retrlines(const std::string &cmd, std::ostream &os=std::cout);
    std::string retrbinary(const std::string &cmd, std::ostream &os);

    std::string storbinary(const std::string &cmd, std::istream &is);
    std::string storlines(const std::string &cmd, std::istream &is);

    std::vector<std::string> nlst(const std::string &path=".");

    std::string remove(const std::string &fname);
    std::string rmd(const std::string &dirname);
    std::string mkd(const std::string &dirname);
    std::string rename(const std::string &from, const std::string &to);

    std::string cwd(const std::string &path);
    std::string pwd();

    std::string dir(const std::string& path=".", std::ostream &os = std::cout);

    socketpp::Socket& transfercmd(const std::string &cmd);
    void set_pasv(bool p);

    std::string quit();
    void close();

    inline void settimeout(double time) { _sock.settimeout(time); }

private:
    socketpp::Socket _sock;
    bool _pasv;

    std::string _readAnswer();
    std::string _getLine(std::string&);
    void _getAddress(const std::string&, std::string&, socketpp::port_t&);
};

};

#endif
