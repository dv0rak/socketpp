#include "ftp.h"

namespace ftplib {
    
FTP::FTP(const std::string &host, const std::string &user, const std::string &passwd)
{
    _pasv = true;
    _sock.open(socketpp::sock_stream);

    if(host != "") {
        connect(host);
        if(user != "") {
            login(user, passwd);
        }
    }
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
        resp >> ret[ret.size()-1];
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
    int a = resp.find("\"");
    return resp.substr(a+1, resp.rfind("\"")-a-1);
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
        char line[BUFSIZ];
        is.getline(line, sizeof line);
        buf += line;
        buf += "\r\n";
    }
    s.send(buf.substr(0,buf.size()-2));
    s.close();
    return _readAnswer();
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
        
void FTP::set_pasv(bool p)
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

std::string FTP::_getLine(std::string& msg)
{
    int n = msg.find("\r\n");
    std::string ret = msg.substr(0, n);
    msg.erase(0, n+2);
    return ret;
}

void FTP::_getAddress(const std::string& resp, std::string& ip, socketpp::port_t& port)
{
    std::string buf = resp.substr(resp.find('(')+1,resp.find(')'));
    int n = buf.find(','); ip += buf.substr(0,n) + '.'; buf.erase(0,n+1);
    n = buf.find(','); ip += buf.substr(0,n) + '.'; buf.erase(0,n+1);
    n = buf.find(','); ip += buf.substr(0,n) + '.'; buf.erase(0,n+1);
    n = buf.find(','); ip += buf.substr(0,n); buf.erase(0,n+1);
    n = buf.find(','); port = (atoi(buf.substr(0,n).c_str())<<8) + atoi(buf.substr(n+1).c_str());
}

};
