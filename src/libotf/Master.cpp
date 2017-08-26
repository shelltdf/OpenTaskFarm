
#include <libotf/Master.h>


//NodeLink::NodeLink()
//{
//}
//
//NodeLink::~NodeLink()
//{
//}
//
//void NodeLink::connentToNode(mg_connection* s)
//{
//}
//
//void NodeLink::disconnentToNode(mg_connection* s)
//{
//}
//
//void NodeLink::recvFromNode(mg_connection* s, char* data, int len)
//{
//}

std::map< mg_connection*, NodeAddr*> NodeAddr::Connection_NodeAddr;


Master::Master(mg_mgr* mgr)
    :mMgr(mgr)
{
}

Master::~Master()
{
}

bool Master::loadConfig(const std::string& filename)
{
    //load file
    std::ifstream ifs;
    ifs.open(filename);
    if (!ifs.good())
    {
        std::cout << filename.c_str() << " is no exisit" << std::endl;
        return false;
    }

    //parser json
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(ifs, root, false))
    {
        std::cout << filename.c_str() << " parser error" << std::endl;
        return false;
    }


    //nodes
    Json::Value value_nodes = root["nodes"];
    for (int i = 0; i < value_nodes.size(); i++)
    {
        std::vector< std::string > sl = split(value_nodes[i].asString(), ":");

        if (sl.size() == 2)
        {
            NodeAddr* na = new NodeAddr(mMgr, sl[0], atoi(sl[1].c_str()));
            //na.IP = sl[0];
            //na.port = atoi(sl[1].c_str());

            mNodeAddrList.push_back(na);
        }
        else
        {
        }
    }

    return false;
}

void Master::update()
{

    //try connet node
    for (size_t i = 0; i < mNodeAddrList.size(); i++)
    {
        if (
            //mNodeAddrList[i]->getState() == 1
            //&& mNodeAddrList[i]->getState() != 3
            mNodeAddrList[i]->getState() != 0
            )
        {
            mNodeAddrList[i]->connent();
        }
    }


    //state change
    if (prestate != state)
    {
        if (state == 0)
        {
            //stop all
            std::cout << "done all" << std::endl;

        }
        if (state == 1)
        {
            //start all
            std::cout << "start all" << std::endl;

        }
        prestate = state;
    }


    //running loop
    if (state == 1)
    {
        bool all_task_is_done = true;

        for (auto it = mTaskJsonStringList.begin(); it != mTaskJsonStringList.end(); it++)
        {
            TaskString& ts = it->second;

            if (ts.status == 0)
            {
                all_task_is_done = false;

                //get one idle node
                NodeAddr* na = findIdleNode();
                if (na)
                {
                    std::string str1 = "{\"dispatch_task\":{\"taskid\":\"" + it->first + "\",\"task\":" + ts.mTaskJson + "}}";
                    na->sendToNode(str1);

                    std::string str2 = "{\"start_task\":{}}";
                    na->sendToNode(str2);

                    ts.mIP = na->getIP();
                    ts.mPort = na->getPort();
                    ts.status = 1;
                    na->mNodeWorkStateWait = true;
                }
            }
            else if (ts.status == 1)
            {
                all_task_is_done = false;

                NodeAddr* na = findNode(ts.mIP, ts.mPort);
                if (na)
                {
                    // maybe bug
                    if (!na->mNodeWorkStateWait
                        && na->getNodeWorkState() == 0 
                        && na->getState() == 0)
                    {
                        ts.status = 2;
                    }
                }
            }
            else if (ts.status == 2)
            {
                //nothing to do
            }
            else if (ts.status == 3)
            {
                //nothing to do
            }
        }

        if (all_task_is_done)
        {
            state = 0;
        }
    }


    //query node state
    static int count = 0;
    if (count > 1000 * 5)
    {
        for (auto it = mNodeAddrList.begin(); it != mNodeAddrList.end(); it++)
        {
            if ((*it)->getState() == 0)
            {
                std::string str = "{\"query_node\":{}}";
                (*it)->sendToNode(str);
            }
        }
        count = 0;
    }
    count++;

}

//void Master::connentToNode(mg_connection* s)
//{
//}
//
//void Master::disconnentToNode(mg_connection* s)
//{
//}
//
//void Master::recvFromNode(mg_connection* s, char* data, int len)
//{
//}

void Master::connentFormCL(mg_connection* nc)
{
    //nothing to do
}

void Master::disconnentFormCL(mg_connection* nc)
{
    //nothing to do
}

void Master::recvFromCL(mg_connection* nc, char* data, int len)
{

    //copy string
    std::string str;
    str.resize(len + 1);
    str[len] = '\0';
    memcpy(&str[0], (void*)data, len);
    printf("cmd : %s\n", str.c_str());

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


    //task group
    //if (str == "add_task_group")
    //{
    //    //send(get_client_info());
    //}
    if (cmd == "query_task_group")
    {
        sendToCL(nc,query_task_group());
    }
    if (cmd == "clear_task_group")
    {
        sendToCL(nc, clear_task_group());
    }

    //task
    if (cmd == "add_task")
    {
        Json::Value taskid_value = value["taskid"];
        Json::Value task_value = value["task"];

        Json::FastWriter writer;
        std::string task_json = writer.write(task_value);

        sendToCL(nc, add_task(taskid_value.asString(), task_json));
    }
    if (cmd == "delete_task")
    {
        Json::Value taskid_value = value["taskid"];

        sendToCL(nc, delete_task(taskid_value.asString()));
    }
    if (cmd == "query_task")
    {
        sendToCL(nc, query_task());
    }
    if (cmd == "order_task")
    {
        sendToCL(nc, order_task());
    }
    if (cmd == "list_task")
    {
        sendToCL(nc, list_task());
    }


    //start
    if (cmd == "start_task")
    {
        sendToCL(nc, start_task());
    }
    if (cmd == "start_task_group")
    {
        sendToCL(nc, start_task_group());
    }

    //restart
    if (cmd == "restart_task")
    {
        sendToCL(nc, restart_task());
    }
    if (cmd == "restart_task_group")
    {
        sendToCL(nc, restart_task_group());
    }

    // node 
    if (cmd == "add_node_address")
    {
        sendToCL(nc, add_node_address());
    }
    if (cmd == "delete_node_address")
    {
        sendToCL(nc, delete_node_address());
    }
    if (cmd == "query_node_state")
    {
        sendToCL(nc, query_node_state());
    }

}




std::string Master::add_node_address()
{
    return "{\"r_add_node_address\":{\"status\":false}}";
}
std::string Master::delete_node_address()
{
    return "{\"r_delete_node_address\":{\"status\":false}}";
}
std::string Master::query_node_state()
{
    return "{\"r_query_node_state\":{\"status\":false}}";
}

std::string Master::query_task_group()
{
    return "{\"r_query_task_group\":{\"status\":false}}";
}
std::string Master::clear_task_group()
{
    if(state == 0)
    {
        mTaskJsonStringList.clear();

        return "{\"r_clear_task_group\":{\"status\":true}}";
    }

    return "{\"r_clear_task_group\":{\"status\":false}}";
}

std::string Master::add_task(std::string name, std::string json)
{
    auto it = mTaskJsonStringList.find(name);
    if (it== mTaskJsonStringList.end())
    {
        mTaskJsonStringList[name] = TaskString(json);

        return "{\"r_add_task\":{\"status\":true}}";
    }

    return "{\"r_add_task\":{\"status\":false}}";
}
std::string Master::delete_task(std::string name)
{
    auto it = mTaskJsonStringList.find(name);
    if (it != mTaskJsonStringList.end())
    {
        mTaskJsonStringList.erase(it);

        return "{\"r_delete_task\":{\"status\":true}}";
    }

    return "{\"r_delete_task\":{\"status\":false}}";
}
std::string Master::query_task()
{
    return "{\"r_query_task\":{\"status\":false}}";
}
std::string Master::order_task()
{
    return "{\"r_order_task\":{\"status\":false}}";
}

std::string Master::list_task()
{
    return "{\"r_list_task\":{\"status\":false}}";
}

std::string Master::start_task()
{
    return "{\"r_start_task\":{\"status\":false}}";
}
std::string Master::start_task_group()
{
    state = 1;

    return "{\"r_start_task_group\":{\"status\":false}}";
}

std::string Master::restart_task()
{
    return "{\"r_restart_task\":{\"status\":false}}";
}
std::string Master::restart_task_group()
{
    state = 1;

    return "{\"r_restart_task_group\":{\"status\":false}}";
}

