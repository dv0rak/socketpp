#include "ftp.h"
#include <sstream>
#include <regex.h>
#include <cstdlib>

namespace ftplib {
    
FTP::FTP(const std::string &host, const std::string &user, const std::string &passwd, double timeout)
{
    _pasv = true;
    _sock.open(socketpp::sock_stream);

    if(host != "") {
        connect(host);
        if(user != "") {
            login(user, passwd);
        }
    }
    settimeout(timeout);
}

std::string FTP::remove(const std::string &fname)
{
    return sendcmd("DELE "+fname);
}

std::string FTP::rmd(const std::string &dirname)
{
    return sendcmd("RMD "+dirname);
}

std::string FTP::mkd(const std::string &dirname)
{
    return sendcmd("MKD "+dirname);
}

std::string FTP::rename(const std::string &from, const std::string &to)
{
    sendcmd("RNFR "+from);
    return sendcmd("RNTO "+to);
}

std::string FTP::connect(const std::string &host, socketpp::port_t port)
{
    _sock.connect(host, port);
    return _readAnswer();
}

std::string FTP::login(const std::string &user, const std::string &passwd)
{
    sendcmd("USER "+user);
    return sendcmd("PASS "+ ((user=="anonymous" && passwd=="")? "anonynous@": passwd));
}
    
std::string FTP::sendcmd(const std::string &cmd)
{
    _sock.send(cmd+"\r\n");
    return _readAnswer();
}

std::vector<std::string> FTP::nlst(const std::string &path)
{
    std::vector<std::string> ret;
    std::stringstream resp;
    retrlines("NLST "+path,resp);
    while(resp.good()) {
        ret.resize(ret.size()+1);
        resp >> ret.back();
    }
    ret.resize(ret.size()-1);
    return ret;
}

std::string FTP::cwd(const std::string &path)
{
    return sendcmd("CWD "+path);
}

std::string FTP::pwd()
{
    std::string resp = sendcmd("PWD");
    std::string pattern = "\"(.*)\"";
    std::vector<std::string> dir;
    
    if(_regex(resp, pattern, 2, dir) < 0) {
        throw error_proto("Unexpected reply to PWD command :\n" +resp);
    }
    return dir[1];
}

std::string FTP::retrlines(const std::string &cmd, std::ostream &os)
{
    sendcmd("TYPE A");
    socketpp::Socket s = transfercmd(cmd);
    sendcmd(cmd);

    std::string buf;
    while(s.recv(buf, BUFSIZ) != 0) {
        while(!buf.empty()) {
            os << _getLine(buf) <<std::endl;
        }
    }
    s.close();
    return _readAnswer();
}
    
std::string FTP::retrbinary(const std::string &cmd, std::ostream &os)
{
    sendcmd("TYPE I");
    socketpp::Socket s = transfercmd(cmd);
    sendcmd(cmd);

    int n;
    char buf[BUFSIZ];
    while((n=s.recv(buf, sizeof buf)) != 0) {
        os.write(buf, n);
    }
    s.close();
    return _readAnswer();
}

std::string FTP::storbinary(const std::string &cmd, std::istream &is)
{
    sendcmd("TYPE I");
    socketpp::Socket s = transfercmd(cmd);
    sendcmd(cmd);

    char buf[BUFSIZ];
    while(is.good()) {
        is.read(buf, sizeof buf);
        s.send(buf, is.gcount());
    }
    s.close();
    return _readAnswer();
}

std::string FTP::storlines(const std::string &cmd, std::istream &is)
{
    sendcmd("TYPE A");
    socketpp::Socket s = transfercmd(cmd);
    sendcmd(cmd);

    std::string buf;
    while(is.good()) {
        std::string line;
        getline(is, line);
        buf += line + "\r\n";
    }
    s.send(buf.substr(0,buf.size()-2));
    s.close();
    return _readAnswer();
}

std::string FTP::dir(const std::string& path, std::ostream &os)
{
    return retrlines("LIST " + path, os);
}

socketpp::Socket& FTP::transfercmd(const std::string &cmd)
{
    socketpp::Socket *s;
    if(_pasv) {
        std::string ip;
        socketpp::port_t port;

        _getAddress(sendcmd("PASV"), ip, port);
        s = new socketpp::Socket(socketpp::sock_stream);
        s->connect(ip, port);
    } else {
        throw error_proto("Not yet implemented!\n");
    }
    return *s;
}
        
void FTP::set_pasv(bool p) throw()
{
    _pasv = p;
}

std::string FTP::quit()
{
    std::string ret = sendcmd("QUIT");
    close();
    return ret;
}

void FTP::close()
{
    _sock.close();
}

std::string FTP::_readAnswer()
{
    std::string msg;
    _sock.recv(msg, BUFSIZ);

    int code = atoi(msg.substr(0,3).c_str());
    if(code<=100 || code>=599) throw error_proto(msg);
    if(code<=599 && code>=500) throw error_perm(code, msg);
    if(code<=499 && code>=400) throw error_temp(code, msg);
    
    return msg;
}

std::string FTP::_getLine(std::string& msg) throw()
{
    int n = msg.find("\r\n");
    std::string ret = msg.substr(0, n);
    msg.erase(0, n+2);
    return ret;
}

int FTP::_regex(const std::string &str, const std::string &pattern, int nmatch, std::vector<std::string> &pmatch) throw()
{
    regex_t re;
    regmatch_t *_pmatch = new regmatch_t[nmatch];   
 
    if(::regcomp(&re, pattern.c_str(), REG_EXTENDED) != 0) return -1;
    if(::regexec(&re, str.c_str(), nmatch, _pmatch, 0) != 0) return -1;

    pmatch.clear();
    for(int i=0; i<nmatch; i++) {
        pmatch.push_back( str.substr(_pmatch[i].rm_so,_pmatch[i].rm_eo-_pmatch[i].rm_so) );
    }
    delete[] _pmatch;
    return 0;
} 

void FTP::_getAddress(const std::string& resp, std::string& ip, socketpp::port_t& port)
{
    std::vector<std::string> tok;
    std::string pattern = "\\(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])," \
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])," \
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])," \
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])," \
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])," \
                          "([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\)";
    
    if(_regex(resp, pattern, 7, tok) < 0) {
        throw error_proto("Unexpected reply to PASV command :\n" +resp);
    }
    ip = tok[1] +"."+ tok[2]+"."+tok[3]+"."+tok[4];
    port = (atoi(tok[5].c_str()) <<8) + atoi(tok[6].c_str());
}

};
