
#include <libotf/SimpleMaster.h>

SimpleMaster::SimpleMaster()
{
}

SimpleMaster::~SimpleMaster()
{
}

//void SimpleMaster::connent()
//{
//}
//
//void SimpleMaster::disconnent()
//{
//}
//
//void SimpleMaster::recv(mg_connection* nc, char* data, int len)
//{
//}

void SimpleMaster::cmd(mg_connection* nc, std::string cmdline)
{
    //parser input string
    std::vector< std::string > sl = split(cmdline, " ");
    if (sl.size() == 0)
    {
        std::cout << "empty command" << std::endl;
    }


    //help
    if (sl[0] == "?")
    {
        std::cout << " help info :" << std::endl;
        std::cout << "    ? - print help info" << std::endl;

        //node manager
        std::cout << "    query_node - query node state" << std::endl;

        //task manager
        std::cout << "    dispatch_task filename.json - dispatch task to node" << std::endl;
        std::cout << "    start_task - start task on node" << std::endl;
        std::cout << "    stop_task - stop task on node" << std::endl;


        return;
    }

    if (sl[0] == "query_node")
    {
        std::string str = "{\"query_node\":{}}";
        send(nc, str);
        return;
    }


    if (sl[0] == "dispatch_task")
    {
        if (sl.size() != 2)
        {
            std::cout << "dispatch_task filename.json" << std::endl;
            return;
        }

        //read json file
        std::ostringstream task_string;
        std::ifstream file;
        file.open(sl[1]);
        if (file.good())
        {
             task_string << file.rdbuf();

             //parser json
             Json::Reader reader;
             Json::Value root;
             if (!reader.parse(task_string.str(), root, false))
             {
                 std::cout << task_string.str().c_str() << " parser error" << std::endl;
                 return;
             }
             std::string taskid = root["setting"]["taskid"].asString();

             //send
             std::string str = "{\"dispatch_task\":{\"taskid\":\"" + taskid + "\",\"task\":" + task_string.str() + "}}";
             send(nc, str);
        }
        else
        {
            std::cout << "bad file" << std::endl;
        }
        
        return;
    }

    if (sl[0] == "start_task")
    {
        std::string str = "{\"start_task\":{}}";
        send(nc, str);
        return;
    }

    if (sl[0] == "stop_task")
    {
        std::string str = "{\"stop_task\":{}}";
        send(nc, str);
        return;
    }


    std::cout << "bad command" << std::endl;

}
