
/*
*   otf node
*
*   contain a websocket server , port 7777
*   subprocess manager
*/


#include < iostream >

#include <libotf/Node.h>

extern "C"
{
#include "mongoose.h"
}


Node* node = 0;
static sig_atomic_t s_signal_received = 0;
//static const char *s_http_port = "7777";
static unsigned short s_http_port = 7777;

static void signal_handler(int sig_num)
{
    signal(sig_num, signal_handler);  // Reinstantiate signal handler
    s_signal_received = sig_num;
}

static int is_websocket(const struct mg_connection *nc)
{
    return nc->flags & MG_F_IS_WEBSOCKET;
}

void sendfunc(mg_connection* nc, std::string str)
{

    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.size());
}


static void broadcast(struct mg_connection *nc, const struct mg_str msg)
{
    struct mg_connection *c;
    char buf[500];
    char addr[32];
    mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

    snprintf(buf, sizeof(buf), "%s %.*s", addr, (int)msg.len, msg.p);
    printf("%s\n", buf); /* Local echo. */
    for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c))
    {
        if (c == nc) continue; /* Don't send to the sender. */
        mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    switch (ev)
    {
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
    {
        /* New websocket connection. Tell everybody. */
        //broadcast(nc, mg_mk_str("++ joined"));

        printf("master joined\n");
        node->connent(nc);
        break;
    }
    case MG_EV_WEBSOCKET_FRAME:
    {
        struct websocket_message *wm = (struct websocket_message *) ev_data;
        /* New websocket message. Tell everybody. */
        //struct mg_str d = { (char *)wm->data, wm->size };
        //broadcast(nc, d);

        node->recv(nc, (char *)wm->data, wm->size);
        break;
    }
    case MG_EV_CLOSE:
    {
        /* Disconnect. Tell everybody. */
        if (is_websocket(nc))
        {
            //broadcast(nc, mg_mk_str("-- left"));

            printf("master left\n");
            node->disconnent(nc);
        }
        break;
    }
    }
}


int main(int argc, char** argv)
{
    struct mg_mgr mgr;
    struct mg_connection *nc;

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    //setvbuf(stdout, NULL, _IOLBF, 0);
    //setvbuf(stderr, NULL, _IOLBF, 0);

    mg_mgr_init(&mgr, NULL);

    do
    {
        char str_port[16];
        sprintf(str_port, "%d", s_http_port);
        nc = mg_bind(&mgr, str_port, ev_handler);

        if (!nc)s_http_port++;

    } while (!nc);

    mg_set_protocol_http_websocket(nc);

    node = new Node();
    node->send = sendfunc;

    printf("Started on port %d\n", s_http_port);
    while (s_signal_received == 0)
    {
        mg_mgr_poll(&mgr, 1);

        node->update();
    }
    mg_mgr_free(&mgr);

    delete node;
    node = 0;

    return 0;
}

