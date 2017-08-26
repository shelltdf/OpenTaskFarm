
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

#include <libotf/SimpleMaster.h>

bool isDone = false;


SimpleMaster* cmdline = 0;

#include "mongoose.h"

static int s_done = 0;
static int s_is_connected = 0;

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
            printf("-- Connection error: %d\n", status);
        }
        break;
    }
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    {
        printf("-- Connected\n");
        s_is_connected = 1;
        break;
    }
    case MG_EV_WEBSOCKET_FRAME:
    {
        printf("%.*s\n", (int)wm->size, wm->data);
        break;
    }
    case MG_EV_CLOSE:
    {
        if (s_is_connected) printf("-- Disconnected\n");
        s_done = 1;
        break;
    }
    }//switch
}

void sendfunc(mg_connection* nc, std::string str)
{

    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
}

int main(int argc, char** argv)
{
    // print welcom string
    std::cout << "welcom to OpenTaskFarm" << std::endl;

    //
    struct mg_mgr mgr;
    struct mg_connection *nc;
    const char *chat_server_url = "ws://127.0.0.1:7777";

    //init
    mg_mgr_init(&mgr, NULL);

    //
    cmdline = new SimpleMaster();
    cmdline->send = sendfunc;

    // try to connet localhost port 6666
    nc = mg_connect_ws(&mgr, ev_handler, chat_server_url, "ws_chat", NULL);
    if (nc == NULL)
    {
        fprintf(stderr, "Invalid address\n");
        return 1;
    }

    // poll once
    mg_mgr_poll(&mgr, 1);


    //input thread
    std::mutex m;
    std::string cmdline_string;
    std::thread t([&]()
    {
        while (!isDone)
        {
            std::string t_cmdline_string;
            //std::cout << ">";
            std::getline(std::cin, t_cmdline_string);

            m.lock();
            cmdline_string = t_cmdline_string;
            m.unlock();
        }
    });


    // main while
    while (!isDone)
    {
        //
        mg_mgr_poll(&mgr, 1);

#if 0
        // input comand line
        std::string cmdline_string;
        std::cout << ">";
        //std::cin >> cmdline_string;
        std::getline(std::cin, cmdline_string);
#endif

        m.lock();
        if (cmdline_string.size() > 0)
        {
            //do comand line
            cmdline->cmd(nc, cmdline_string);

            cmdline_string = "";
        }
        m.unlock();

    }//while

    mg_mgr_free(&mgr);

    return 0;
}

