#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "libubox/uloop.h"
#include <libubox/ustream.h> 
#include "libubox/usock.h"



static struct uloop_fd u_fd;
char *port = "10010";


static void u_callback(struct uloop_fd *ufd, unsigned int event)
{
    char buf[1024];
    int len;

    while(1) {
        memset(buf, 0, sizeof(buf));
        len = recv(ufd->fd, buf, 1024, 0);
        if (len < 0) {
            if (errno == EINTR)
                continue;
            break;
        }
        if (!len)
            break;
        buf[len] = 0;
        printf("buf: %s\r\n", buf);
    }
}


int main(void)
{
    u_fd.fd = usock(USOCK_UNIX | USOCK_SERVER | USOCK_IPV4ONLY | USOCK_NUMERIC, "127.0.0.1", port);
    u_fd.cb = u_callback;

    uloop_init();
    uloop_fd_add(&u_fd, ULOOP_READ);
    uloop_run();
    
    return 0;
}
