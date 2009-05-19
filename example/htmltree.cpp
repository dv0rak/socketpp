/**
 *  This program creates in every subdirectory of a given path of a given ftp space
 *  a index.html file which lists all the existing files in the directory
 */

#include <socket++.h>
using namespace ftplib;
using namespace std;

void do_tree(FTP &ftp)
{
    stringstream index;
    index << "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n";
    index << "<html>\n<head>\n\t<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\">\n";
    index << "\t<title>" << ftp.pwd() <<"</title>\n</head>\n<body>\n\t<div>\n";
    index << "\t<- <a href=\"..\">Parent Directory</a><br>\n";

    vector<string> list = ftp.nlst();
    for(vector<string>::iterator it=list.begin(); it!=list.end(); it++) {
        if(*it == "index.html") continue;
        try {
            ftp.cwd(*it);
            do_tree(ftp);
            ftp.cwd("..");
            index << "\t-> ";
        } 
        catch(error_perm &e) {
            index << "\t";
        }
        index << "<a href=\""<< *it <<"\">"<< *it <<"</a> <br>\n";
    }

    index << "\t</div>\n</body>\n</html>";
    ftp.storlines("STOR index.html", index);
    return;
}

int main(int argc, char **argv)
{
    FTP ftp;
    string user, pass;

    if(argc < 3) {
        cout << argv[0] << " <host> <path> [<port>]"<<endl;
        exit(-1);
    }
   
    if(argc == 4) ftp.connect(argv[1], atoi(argv[3]));
    else ftp.connect(argv[1]);

    cout <<"Username: ";
    cin >> user;
    cout <<"Password: ";
    cin >> pass;

    ftp.login(user, pass);
    ftp.cwd(argv[2]);
    do_tree(ftp);

    ftp.quit();
    exit(0);
}
