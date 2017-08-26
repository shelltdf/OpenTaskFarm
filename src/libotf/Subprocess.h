
#ifndef _OTF_SUBPROCESS_H_
#define _OTF_SUBPROCESS_H_

#if _WIN32
#include <windows.h>
#endif

#include <string>

class Subprocess
{
public:
    Subprocess();
    ~Subprocess();

    void run(const std::string& cmdline);

    void stop();

    bool isRun();

private:


#if _WIN32
    PROCESS_INFORMATION pi;
#endif

};

#endif // !_OTF_SUBPROCESS_H_


