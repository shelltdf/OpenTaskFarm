
#ifndef _OTF_TASK_H_
#define _OTF_TASK_H_

#include <iostream>
#include <string>
#include <vector>
#include <direct.h>  

#include <libotf/Subprocess.h>

#include "json/json.h"

/*

{
"setting": {
"taskID": "textproc",
"workdir": "textproc"
},
"downloads": [{
"source": "ftp://name.exe",
"dest": "textproc.exe",
"log": "log_downloads.txt"
},
{
"source": "namelist.txt",
"dest": "namelist.txt",
"log": "log_downloads.txt"
}],
"processes": [{
"source": "textproc.exe -i namelist.txt -o outname.txt -log log.txt",
"dest": "outname.txt",
"log": "log_processes.txt"
}],
"uploads": [{
"source": "outname.txt",
"dest": "ftp://outname.txt",
"log": "log_uploads.txt"
}]
}


*/


struct TaskSetting
{
    std::string taskID;
    std::string workdir;
};

struct TaskOperationData
{
    std::string source;
    //std::string dest;
    //std::string log;

    bool running = false;
    bool done = false;
};

class Task
{
public:
    Task();
    ~Task();

    bool fromJson(const std::string json);

    std::string toJson();

    //void clean();

    void run();

    void stop();

    bool isRun();

    void update();

private:

    int pre_state = 0; //0-idle 1-running
    int state = 0; //0-idle 1-running
    bool done = false;

    Subprocess* sp = 0;

    //setting
    TaskSetting setting;

    // download
    std::vector<TaskOperationData> downloads;

    // processing
    std::vector<TaskOperationData> processes;

    // upload
    std::vector<TaskOperationData> uploads;


    std::string replaceVariable(std::string str) const;

    bool/*is_all_done*/ doOperation(std::vector<TaskOperationData>& data_list) const;

};

#endif // !_OTF_TASK_H_
