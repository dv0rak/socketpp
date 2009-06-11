#ifndef __FTP_H
#define __FTP_H 1

#include "Socket.h"
#include <iostream>
#include <vector>

namespace ftplib {

///@brief	exception thrown when a FTP error code in the range 400-499 is received
class error_temp : public std::exception {
public:
    error_temp(int code, const std::string &msg) throw()
    {
        _code = code;
        _msg = msg;
    }
    ~error_temp() throw() {}

    virtual const char * what() const throw()
    {
        return _msg.c_str(); 
    }
private:
    int _code;
    std::string _msg;
};

///@brief	exception thrown when a FTP error code in the range 500-599 is received
class error_perm : public std::exception {
public:
    error_perm(int code, const std::string &msg) throw()
    {
        _code = code;
        _msg = msg;
    }
    ~error_perm() throw() {}
    
    virtual const char * what() const throw()
    {
        return _msg.c_str(); 
    }
private:
    int _code;
    std::string _msg;
};

///@brief	exception thrown when a unexpected reply from the server is received
class error_proto : public std::exception {
public:
    error_proto(const std::string &msg) throw()
    {
        _msg = msg;
    }
    ~error_proto() throw() {}
    
    virtual const char * what() const throw()
    {
        return _msg.c_str(); 
    }
private:
    std::string _msg;
};

///@brief This class tries to imitate the ftplib.FTP python class, with a few differences.
class FTP {
public:
    ///@brief	constructor
    ///@param	host    if not empty it connects to given hostname
    ///@param	user    if not empty it calls FTP::login(user, passw)
    ///@param	passwd	login password 
    ///@param	timeout	if not empty it calls FTP::settimeout(timeout)
    explicit FTP(const std::string &host="", const std::string &user="", const std::string &passwd="", double timeout=0.0);

    ///@brief	connects to given FTP server
    ///@param	host	remote server address
    ///@param	port 	remote server port (21 if empty)
    ///@return	FTP reply
    std::string connect(const std::string &host, socketpp::port_t port=21);
    ///@brief	carries out the FTP login
    ///@param	user	username, "anonymous" if empty
    ///@param	passwd	password, if it's empty and user is "anonymous" it becomes "anonymous@"
    ///@return	FTP reply
    std::string login(const std::string &user="anonymous", const std::string &passwd="");
    ///@brief	sends raw command and returns server response
    ///@param	cmd	FTP command
    ///@return	FTP reply
    std::string sendcmd(const std::string &cmd);
    ///@brief	Retrieve a file or directory listing in ASCII transfer mode
    ///@param	cmd	FTP command, should be something like RETR or LIST
    ///@param	os	std::ostream on which retrieved lines are written, std::cout if empty
    ///@return	FTP reply
    std::string retrlines(const std::string &cmd, std::ostream &os=std::cout);
    ///@brief	Retrieve a file in binary transfer mode
    ///@param	cmd	FTP command, should be something like 'RETR filename'
    ///@param	os	std::ostream on which retrieved data are written
    ///@return	FTP reply
    std::string retrbinary(const std::string &cmd, std::ostream &os);

    ///@brief	Stor a file in binary transfer mode
    ///@param	cmd	FTP command, should be something like 'STOR filename'
    ///@param	is	std::istream from which data are red
    ///@return	FTP reply
    std::string storbinary(const std::string &cmd, std::istream &is);
    ///@brief	Stor a file in ASCII transfer mode
    ///@param	cmd	FTP command, should be something like 'STOR filename'
    ///@param	is	std::istream from which data are red
    ///@return	FTP reply
    std::string storlines(const std::string &cmd, std::istream &is);

    ///@brief	obtains a std::vector containing existing file names in the passed path, through NLST command
    ///@param	path	path to pass to NLST command, "." if empty
    ///@return	file names vector
    std::vector<std::string> nlst(const std::string &path=".");

    ///@brief	remove a file from the server
    ///@param	fname	file to remove
    ///@return	FTP reply
    std::string remove(const std::string &fname);
    ///@brief	remove a direcotry from the server
    ///@param	dirname	directory to remove
    ///@return	FTP reply
    std::string rmd(const std::string &dirname);
    ///@brief	creates a new directory on the server
    ///@param	dirname	directory to remove
    ///@return	FTP reply
    std::string mkd(const std::string &dirname);
    ///@brief	renames a file on the server
    ///@param	from	file to rename
    ///@param	to 	new file name
    ///@return	FTP reply
    std::string rename(const std::string &from, const std::string &to);
    ///@brief	changes working directory
    ///@param	path	directory name
    ///@return	FTP reply
    std::string cwd(const std::string &path);
 
    ///@brief	returns current working directory
    ///@return	current working directory
    std::string pwd();

    ///@brief	lists all the files in the directory, through the LIST command
    ///param	path	directory to list, "." if empty
    ///param	os	std::ostream on which output is written, std::cout if empty
    ///@return	FTP reply
    std::string dir(const std::string& path=".", std::ostream &os = std::cout);

    ///@brief	initiate a transfer over the data connection, using PASV or PORT mode depending on previous calls to set_pasv() (default is PASV), and then send given command
    ///@param	cmd	FTP command to execute
    ///@return	connected FTP-data Socket object
    socketpp::Socket& transfercmd(const std::string &cmd);
    ///@brief	sets either PORT or PASV mode
    ///@param	p	if true PASV, otherwise PORT
    void set_pasv(bool p) throw();

    ///@brief	sends the 'QUIT' command and closes socket
    ///@return	FTP reply
    std::string quit();
    ///@brief	closes socket
    void close();

    ///@brief	sets timeout on all IO socket operations
    ///@param	time	timeout value in seconds, if 0.0 timeout is cancelled
    inline void settimeout(double time) { _sock.settimeout(time); }

private:
    socketpp::Socket _sock;
    bool _pasv;

    std::string _readAnswer();
    std::string _getLine(std::string&) throw();
    void _getAddress(const std::string&, std::string&, socketpp::port_t&);
    int _regex(const std::string &, const std::string &, int, std::vector<std::string> &) throw();
};

};

#endif
