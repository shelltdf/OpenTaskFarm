
#ifndef _OTF_NODE_H_
#define _OTF_NODE_H_

#include <functional>
#include <map>
#include <set>

#include "json/json.h"

extern "C"
{
#include "mongoose.h"
}

#include <libotf/Task.h>


class Node
{
public:
    Node();
    ~Node();

    void connent(mg_connection* nc);

    void disconnent(mg_connection* nc);

    void recv(mg_connection* nc, char* data, int len);

    std::function<void(mg_connection* nc, std::string)> send;

    void update()
    {
        if (task)
        {
            task->update();
        }
    }

private:

    std::set< mg_connection* > mLinkSet;

    Task* task = 0;
    std::string taskid;

    //std::string get_client_info();
    //std::string cleanup_temp_file();
    //std::string get_log_file();
    std::string query_node(std::string ip_and_port);
    std::string dispatch_task(std::string taskid,std::string task_json);
    std::string start_task();
    std::string stop_task();

};


#endif // !_OTF_NODE_H_
