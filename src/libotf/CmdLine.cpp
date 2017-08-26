
#include <libotf/CmdLine.h>

CmdLine::CmdLine()
{
}

CmdLine::~CmdLine()
{
}

void CmdLine::connent()
{
}

void CmdLine::disconnent()
{
}

void CmdLine::recv(mg_connection* nc, char* data, int len)
{
}

void CmdLine::cmd(mg_connection* nc, std::string cmdline)
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
        //std::cout << "    list_node - list master node" << std::endl;

        //task manager
        std::cout << "    add_task filename.json - add task to master" << std::endl;
        std::cout << "    list_task - list task on master" << std::endl;
        std::cout << "    start_task_group - start all task" << std::endl;


        return;
    }

#if 0

    // login
    if (sl[0] == "login")
    {
        if (cheakLogState())
        {
            std::cout << "already login" << std::endl;
        }
        else
        {
            std::string str = "{\"login\":{}}";
            //mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
            send(nc, str);
        }
        return;
    }

    //logout
    if (sl[0] == "logout")
    {
        std::string str = "{\"logout\":{}}";
        //mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
        send(nc, str);

        return;
    }

#endif // 0

    //if (sl[0] == "add_node")
    //{
    //}

    //if (sl[0] == "list_node")
    //{
    //    std::string str = "{\"list_node\":{}}";
    //    send(nc, str);
    //    return;
    //}

    //if (sl[0] == "query_node")
    //{
    //}

    //if (sl[0] == "delete_node")
    //{
    //}

    if (sl[0] == "add_task")
    {
        if (sl.size() != 2)
        {
            std::cout << "add_task filename.json" << std::endl;
            return;
        }

        //read json file
        std::ostringstream task_string;
        std::ifstream file;
        file.open(sl[1]);
        if (file.good())
        {
            //read file
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
            std::string str = "{\"add_task\":{\"taskid\":\"" + taskid + "\",\"task\":" + task_string.str() + "}}";
            send(nc, str);
        }
        else
        {
            std::cout << "bad file" << std::endl;
        }

        return;
    }

    if (sl[0] == "list_task")
    {
        std::string str = "{\"list_task\":{}}";
        send(nc, str);
        return;
    }

    if (sl[0] == "start_task_group")
    {
        std::string str = "{\"start_task_group\":{}}";
        send(nc, str);
        return;
    }

    std::cout << "bad command" << std::endl;

}
