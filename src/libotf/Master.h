
#ifndef _OTF_MASTER_H_
#define _OTF_MASTER_H_

#include <iostream>
#include <functional>
#include <vector>
#include <fstream>
#include <map>

#include "json/json.h"

extern "C"
{
#include "mongoose.h"
}


//class NodeLink
//{
//public:
//    NodeLink();
//    ~NodeLink();
//
//    void connentToNode(mg_connection* nc);
//
//    void disconnentToNode(mg_connection* nc);
//
//    void recvFromNode(mg_connection* nc, char* data, int len);
//
//    std::function<void(std::string)> sendToNode;
//
//private:
//
//};

class NodeAddr
{
public:
    NodeAddr(mg_mgr* mgr,std::string ip, unsigned short port)
        :mMgr(mgr),mIP(ip), mPort(port) {}
    ~NodeAddr() {}

    int getState() { return mConnetState; }
    int getNodeWorkState() { return mNodeWorkState; }

    //std::function<void(std::string)> sendToNode;

    void sendToNode(/*mg_connection* nc, */std::string str)
    {
        if (mNC)
        {
            mg_send_websocket_frame(mNC, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
        }
    }

    void connent() 
    {
        if (mConnetStateWait) return;

        char chat_server_url[128];
        sprintf(chat_server_url,"%s:%d", mIP.c_str(), mPort);

        mNC = mg_connect_ws( mMgr, ev_handler, chat_server_url, "ws_chat", NULL);
        if (mNC == NULL)
        {
            fprintf(stderr, "Invalid address\n");
            return;
        }

        mConnetStateWait = true;
        Connection_NodeAddr[mNC] = this;
    }

    void disconnent() {}

    std::string getIP() { return mIP; }
    unsigned short getPort() { return mPort; }

private:

    std::string mIP = "127.0.0.1";
    unsigned short mPort = 0;

    int mConnetState = 1; // 0-connet 1-disconnet 2-error 3-force close
    bool mConnetStateWait = false;

    //NodeLink* mLink = 0;
    struct mg_connection *mNC = 0;
    mg_mgr* mMgr;

public:
    int mNodeWorkState = 0; // 0-idle 1-running
    bool mNodeWorkStateWait = false;
private:

    void onConnentToNodeError(mg_connection* nc,int status)
    {
        char addr[32];
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
            MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

        if (mConnetState != 2)
        {
            printf("-- Connection error: %d %s\n", status, addr);
        }

        mConnetState = 2;
        mConnetStateWait = false;
    }

    void onConnentToNode(mg_connection* nc)
    {
        char addr[32];
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
            MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

        printf("-- Connected %s\n", addr);
        mConnetState = 0;
        mConnetStateWait = false;
    }

    void onDisconnentToNode(mg_connection* nc)
    {
        char addr[32];
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
            MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

        if (findNodeAddr(nc)->mConnetState == 0)
        {
            printf("-- Disconnected %s\n", addr);
            mConnetState = 1;
        }
        mConnetStateWait = false;
    }

    void onRecvFromNode(mg_connection* nc, char* data, int len)
    {
        mNodeWorkStateWait = false;

        //copy string
        std::string str;
        str.resize(len + 1);
        str[len] = '\0';
        memcpy(&str[0], (void*)data, len);
        printf("node : %s\n", str.c_str());


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

        if (cmd == "r_query_node")
        {
            std::string nodeid = value["nodeid"].asString();
            std::string taskid = value["taskid"].asString();
            int running = value["running"].asBool();

            mNodeWorkState = running;// ? 1 : 0;

        }
        if (cmd == "r_dispatch_task")
        {
            //nothing to do
        }
        if (cmd == "r_start_task")
        {
            //nothing to do
        }
        if (cmd == "r_stop_task")
        {
            //nothing to do
        }

    }




    static std::map< mg_connection*, NodeAddr*> Connection_NodeAddr;
    static NodeAddr* findNodeAddr(mg_connection* nc)
    {
        auto it = Connection_NodeAddr.find(nc);
        if (it != Connection_NodeAddr.end())
        {
            return it->second;
        }
        return 0;
    }
    static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
    {
        struct websocket_message *wm = (struct websocket_message *) ev_data;
        (void)nc;

        switch (ev)
        {
        case MG_EV_CONNECT:
        {
            int status = *((int *)ev_data);
            if (status != 0)
            {
                findNodeAddr(nc)->onConnentToNodeError(nc, status);
            }
            break;
        }
        case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
        {
            findNodeAddr(nc)->onConnentToNode(nc);
            break;
        }
        case MG_EV_WEBSOCKET_FRAME:
        {
            findNodeAddr(nc)->onRecvFromNode(nc, (char*)wm->data, (int)wm->size);
            break;
        }
        case MG_EV_CLOSE:
        {
            findNodeAddr(nc)->onDisconnentToNode(nc);
            break;
        }
        }
    }


};

struct TaskString
{
    TaskString(std::string task_json = "") :mTaskJson(task_json) {}

    std::string mTaskJson;

    int status = 0;// 0-standby 1-running 2-done 3-error

    //run node
    std::string mIP = "";
    unsigned short mPort = 0;

};


class Master
{
public:
    Master(mg_mgr* mgr);
    ~Master();

    bool loadConfig(const std::string& filename);

    void update();


    // to cmdline
    void connentFormCL(mg_connection* nc);

    void disconnentFormCL(mg_connection* nc);

    void recvFromCL(mg_connection* nc, char* data, int len);

    std::function<void(mg_connection* , std::string)> sendToCL;


private:

    std::vector<NodeAddr*> mNodeAddrList;
    mg_mgr* mMgr;

    std::map<std::string, TaskString> mTaskJsonStringList;

    int prestate = 0; // 0-idle 1-running
    int state = 0; // 0-idle 1-running

    std::string add_node_address();
    std::string delete_node_address();
    std::string query_node_state();

    std::string query_task_group();
    std::string clear_task_group();

    std::string add_task(std::string name, std::string json);
    std::string delete_task(std::string name);
    std::string query_task();
    std::string order_task();
    std::string list_task();

    std::string start_task();
    std::string start_task_group();

    std::string restart_task();
    std::string restart_task_group();


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

    NodeAddr* findIdleNode()
    {
        for (auto it = mNodeAddrList.begin(); it != mNodeAddrList.end(); it++)
        {
            if ((*it)->getNodeWorkState() == 0 && (*it)->getState() == 0)
            {
                return *it;
            }
        }
        return 0;
    }

    NodeAddr* findNode(std::string ip, unsigned short port)
    {
        for (auto it = mNodeAddrList.begin(); it != mNodeAddrList.end(); it++)
        {
            if ((*it)->getIP().size() > 0)
            {
                if ((*it)->getIP() == ip && (*it)->getPort() == port)
                {
                    return *it;
                }
            }
        }
        return 0;
    }
};



#endif // !_OTF_MASTER_H_
