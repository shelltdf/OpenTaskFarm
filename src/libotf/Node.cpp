
#include <libotf/Node.h>


Node::Node()
{
}

Node::~Node()
{
}

void Node::connent(mg_connection* nc)
{
    mLinkSet.insert(nc);
}

void Node::disconnent(mg_connection* nc)
{
    auto it = mLinkSet.find(nc);
    if (it != mLinkSet.end())
    {
        mLinkSet.erase(it);
    }
}

void Node::recv(mg_connection* nc, char* data, int len)
{
    //copy string
    std::string str;
    str.resize(len + 1);
    str[len] = '\0';
    memcpy(&str[0], (void*)data, len);
    //printf("cmd : %s\n", str.c_str());


    //parser json
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(str, root, false))
    {
        std::cout << str.c_str() << " parser error" << std::endl;
        return;
    }

    if (root.size() == 0) return;

    auto it = root.begin();
    std::string cmd = it.key().asString();
    Json::Value value = *it;

    //
    //if (cmd == "get_client_info")
    //{
    //    send(get_client_info());
    //}
    //if (cmd == "cleanup_temp_file")
    //{
    //    send(cleanup_temp_file());
    //}
    //if (cmd == "get_log_file")
    //{
    //    send(get_log_file());
    //}
    if (cmd == "query_node")
    {
        char addr[32] = {0};
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
            MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_REMOTE);

        send(nc, query_node(addr));
    }

    if (cmd == "dispatch_task")
    {
        printf("cmd : %s\n", str.c_str());

        Json::Value taskid_value = value["taskid"];
        Json::Value task_value = value["task"];
        //std::string task_json = task_value.asString();

        Json::FastWriter writer;
        std::string task_json = writer.write(task_value);

        send(nc, dispatch_task(taskid_value.asString(), task_json));
    }
    if (cmd == "start_task")
    {
        printf("cmd : %s\n", str.c_str());

        send(nc, start_task());
    }

    if (cmd == "stop_task")
    {
        printf("cmd : %s\n", str.c_str());

        send(nc, stop_task());
    }

}

//std::string Node::get_client_info()
//{
//    return "{\"r_get_client_info\":{\"status\":false}}";
//}
//std::string Node::cleanup_temp_file()
//{
//    return "{\"r_cleanup_temp_file\":{\"status\":false}}";
//}
//std::string Node::get_log_file()
//{
//    return "{\"r_get_log_file\":{\"status\":false}}";
//}
std::string Node::query_node(std::string ip_and_port)
{
    bool running = false;
    if (task)
    {
        running = task->isRun();
    }

    if (running)
    {
        return"{\"r_query_node\":{\"status\":true,\"nodeid\":\"" + ip_and_port + "\",\"taskid\":\"" + taskid + "\",\"running\":1}}";
    }
    //else
    //{
        return "{\"r_query_node\":{\"status\":true,\"nodeid\":\"" + ip_and_port + "\",\"taskid\":\"" + taskid + "\",\"running\":0}}";
    //}

    //return "{\"r_query_node\":{\"status\":false}}";
}

std::string Node::dispatch_task(std::string taskid, std::string task_json)
{
    if (task)
    {
        delete task;
        task = 0;
    }

    task = new Task();
    if (task->fromJson(task_json))
    {
        taskid = taskid;
        return "{\"r_dispatch_task\":{\"status\":true}}";
    }

    if (task)
    {
        delete task;
        task = 0;
    }

    return "{\"r_dispatch_task\":{\"status\":false}}";
}
std::string Node::start_task()
{
    if (task)
    {
        task->run();

        return "{\"r_start_task\":{\"status\":true}}";
    }

    return "{\"r_start_task\":{\"status\":false}}";
}
std::string Node::stop_task()
{
    if (task)
    {
        task->stop();

        if (task)
        {
            delete task;
            task = 0;
        }

        taskid = "";

        return "{\"r_stop_task\":{\"status\":true}}";
    }

    return "{\"r_stop_task\":{\"status\":false}}";
}

