
#ifndef _OTF_CMDLINE_H_
#define _OTF_CMDLINE_H_

#include <iostream>
#include <functional>
#include <fstream>
#include <sstream>
#include <vector>

#include "json/json.h"

extern "C"
{
#include "mongoose.h"
}


class CmdLine
{
public:
    CmdLine();
    ~CmdLine();

    void cmd(mg_connection* nc, std::string cmdline);

    void connent();

    void disconnent();

    void recv(mg_connection* nc,char* data, int len);

    std::function<void(mg_connection* nc, std::string)> send;

private:

    bool isConnet = false;
    bool isLogin = false;

    bool cheakLogState()
    {
        if (!isLogin)
        {
            std::cout << "please login frist" << std::endl;
        }
        return isLogin;
    }


    std::vector< std::string > split(const std::string& s, const std::string& delim)
    {
        std::vector< std::string > ret;

        size_t last = 0;
        size_t index = s.find_first_of(delim, last);
        while (index != std::string::npos)
        {
            std::string str = s.substr(last, index - last);
            if (str.size() > 0)ret.push_back(str);
            last = index + 1;
            index = s.find_first_of(delim, last);
        }
        if (index - last > 0)
        {
            std::string str = s.substr(last, index - last);
            if (str.size() > 0)ret.push_back(str);
        }

        return ret;
    }

};

#endif // !_OTF_CMDLINE_H_
