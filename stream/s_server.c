#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*关键头文件*/
#include <libubox/uloop.h> // uloop相关的操作
#include <libubox/ustream.h> 
#include <libubox/usock.h>

struct client {
    struct sockaddr_in sin;
    struct ustream_fd s;
    int ctr;
};


static struct uloop_fd server;
struct client *next_client = NULL;
static const char *port = "10000";

static void client_read_cb(struct ustream *s, int bytes)
{
    struct client *cl = container_of(s, struct client, s.stream);
    struct ustream_buf *buf = s->r.head;
    char *newline, *str;

    do {
        str = ustream_get_read_buf(s, NULL);
        if (!str)
            break;

        newline = strchr(buf->data, '\n');
        if (!newline)
            break;
        
        *newline = 0;
        ustream_printf(s, "%s\n", str);
        ustream_consume(s, newline + 1 - str);
        cl->ctr += newline + 1 - str;
    }while(1);

    if (s->w.data_bytes > 256 && !ustream_read_blocked(s)) {
        printf("block read bytes: %d\r\n", s->w.data_bytes);
        ustream_set_read_blocked(s, true);
    }

}

static void client_close(struct ustream *s)
{
    struct client *cl = container_of(s, struct client, s.stream);
    printf("Connection close\r\n");
    ustream_free(s);
    close(cl->s.fd.fd);
    free(cl);
}

static void client_notify_state(struct ustream *s)
{
    struct client *cl = container_of(s, struct client, s.stream);
    if (!s->eof)
        return ;
    printf("EOF, pending: %d, total: %d\n", s->w.data_bytes, cl->ctr);
    if (!s->w.data_bytes) 
        return client_close(s);
}

static void client_notify_write(struct ustream *s, int bytes)
{
    printf("Wrote %d bytes, pending: %d\r\n", bytes, s->w.data_bytes);
    if (s->w.data_bytes < 128 && ustream_read_blocked(s)) {
        printf("Unblock read\n");
        ustream_set_read_blocked(s, false);
    }
}

static void server_cb(struct uloop_fd *fd, unsigned int event)
{
    struct client *cl;
    unsigned int sl = sizeof(struct sockaddr_in);
    int sfd;

    if (!next_client) {
        next_client  = calloc(1, sizeof(*next_client));
    }

    cl = next_client;
    sfd = accept(server.fd, (struct sockaddr *)&cl->sin, &sl);  //accept 返回对端的地址信息，改变量和长度指针可以设置为空
    if (sfd < 0) {
        printf("Accept error\r\n");
        return ;
    }
    
    cl->s.stream.string_data = true;
    cl->s.stream.notify_read = client_read_cb;
    cl->s.stream.notify_state = client_notify_state;
    cl->s.stream.notify_write = client_notify_write;
    ustream_fd_init(&cl->s, sfd);
    next_client = NULL;
    printf("new connection\r\n");
}

static int run_server(void)
{
    server.cb = server_cb;
    server.fd = usock(USOCK_TCP| USOCK_SERVER |USOCK_IPV4ONLY | USOCK_NUMERIC, "127.0.0.1", port);
    if (server.fd < 0) {
        printf("usock error\r\n");
        return -1;
    }
    uloop_init();
    uloop_fd_add(&server, ULOOP_READ);
    uloop_run();

    return 0;
}


int main(int argc, char **argv)
{

    run_server();

    return 0;
}