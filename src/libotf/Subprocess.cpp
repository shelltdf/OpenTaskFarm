
#include <libotf/Subprocess.h>


Subprocess::Subprocess()
{
}

Subprocess::~Subprocess()
{
    stop();
}


void Subprocess::run(const std::string& cmdline)
{

#if _WIN32

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcess(NULL,
        (LPSTR)cmdline.c_str(),
        NULL, NULL, TRUE,
        NORMAL_PRIORITY_CLASS,// | CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si, &pi))
    {
        //CloseHandle(pi.hThread);
        //CloseHandle(pi.hProcess);
    }

    // Wait until child process exits.
    //WaitForSingleObject(pi.hProcess, INFINITE);

#endif

}


void Subprocess::stop()
{

#if _WIN32

    TerminateProcess(pi.hProcess, 300);
    //ExitProcess(1001);

#endif

}


bool Subprocess::isRun()
{
#if _WIN32

    DWORD s = WaitForSingleObject(pi.hProcess, 1);

    if (s == WAIT_ABANDONED)
    {
        return false;
    }
    if (s == WAIT_OBJECT_0)
    {
        return false;
    }
    if (s == WAIT_TIMEOUT)
    {
        return true;
    }
    if (s == WAIT_FAILED)
    {
        return false;
    }
    return false;

#endif
}

