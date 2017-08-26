
#include <iostream>
#include <string>

#include <libotf/CmdLine.h>

bool isDone = false;


CmdLine* cmdline = 0;

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
#if 0
    case MG_EV_POLL: {
        char msg[500];
        int n = 0;
#ifdef _WIN32 /* Windows console input is special. */
        INPUT_RECORD inp[100];
        HANDLE h = GetStdHandle(STD_INPUT_HANDLE);
        DWORD i, num;
        if (!PeekConsoleInput(h, inp, sizeof(inp) / sizeof(*inp), &num)) break;
        for (i = 0; i < num; i++) {
            if (inp[i].EventType == KEY_EVENT &&
                inp[i].Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
                break;
            }
        }
        if (i == num) break;
        if (!ReadConsole(h, msg, sizeof(msg), &num, NULL)) break;
        /* Un-unicode. This is totally not the right way to do it. */
        for (i = 0; i < num * 2; i += 2) msg[i / 2] = msg[i];
        n = (int)num;
#else /* For everybody else, we just read() stdin. */
        fd_set read_set, write_set, err_set;
        struct timeval timeout = { 0, 0 };
        FD_ZERO(&read_set);
        FD_ZERO(&write_set);
        FD_ZERO(&err_set);
        FD_SET(0 /* stdin */, &read_set);
        if (select(1, &read_set, &write_set, &err_set, &timeout) == 1) {
            n = read(0, msg, sizeof(msg));
        }
#endif
        if (n <= 0) break;
        while (msg[n - 1] == '\r' || msg[n - 1] == '\n') n--;
        mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, msg, n);
        break;
    }
#endif
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
    const char *chat_server_url = "ws://127.0.0.1:6666";

    //init
    mg_mgr_init(&mgr, NULL);

    //
    cmdline = new CmdLine();
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


    // main while
    while (!isDone)
    {
        //
        mg_mgr_poll(&mgr, 1);

        // input comand line
        std::string cmdline_string;
        std::cout << ">";
        //std::cin >> cmdline_string;
        std::getline(std::cin, cmdline_string);


        //do comand line
        cmdline->cmd(nc, cmdline_string);

    }//while

    mg_mgr_free(&mgr);

    return 0;
}

