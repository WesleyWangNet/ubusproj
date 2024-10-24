#include <stdio.h>


/*关键头文件*/
#include <libubox/uloop.h> // uloop相关的操作
#include <libubox/blobmsg_json.h>
#include <libubus.h>

#define UBUS_EVENT_ADD_DEVICE "add_device"
#define UBUS_EVENT_REMOVE_DEVICE "remove_device"

static const char *cli_path = NULL;
static struct ubus_context *ctx = NULL;


static void ubus_add_fd(void)
{
    ubus_add_uloop(ctx);

}

/* ubus timer sample 
*  struct uloop_timeout 
*  uloop_timeout_set
*/
static void ubus_reconn_timer(struct uloop_timeout *timeout)
{
    static struct uloop_timeout retry = {
        .cb = ubus_reconn_timer,
    };
    int t = 2;

    if (ubus_reconnect(ctx, cli_path) != 0) {
        uloop_timeout_set(&retry, t * 1000);
        return;
    }
    ubus_add_fd();
}

static void ubus_connection_lost(struct ubus_context *ctx)
{
    ubus_reconn_timer(NULL);
}

static int ubus_init(const char *path)
{
    uloop_init();
    cli_path = path;

    ctx = ubus_connect(path);
    if (!ctx) {
        printf("ubus connected failed\r\n");
        return -1;
    }

    ctx->connection_lost = ubus_connection_lost;
    ubus_add_fd();

    return 0;
}

static void ubus_uninit(void)
{
    if (ctx) ubus_free(ctx);
}

static void ubus_probe_device_event(struct ubus_context *ctx, struct ubus_event_handler *ev, const char *type, struct blob_attr *msg)
{
    char *str;
    if (!msg) {
        return;
    }
    str = blobmsg_format_json(msg, true);
    printf("{\"%s\":\"%s\"}\r\n", type, str);
    free(str);
}
//struct ubus_event_handler listener; //必须时静态你或全局定义
static int client_register()
{
    static struct ubus_event_handler listener;  //必须时静态的定义或者时全局的定义
    int ret = 0;

    memset(&listener, 0, sizeof(struct ubus_event_handler));
    listener.cb = ubus_probe_device_event;
    ret = ubus_register_event_handler(ctx, &listener, UBUS_EVENT_ADD_DEVICE);
    if (ret) {
        printf("register error\r\n");
        return -1;
    }

    ret = ubus_register_event_handler(ctx, &listener, UBUS_EVENT_REMOVE_DEVICE);
    if (ret) {
        printf("register error\r\n");
        return -1;
    }
    return 0;
}


int main(int argc, char **argv)
{

    if (ubus_init(argv[1]) != UBUS_STATUS_OK) {
        return -1;
    }

    client_register();

    uloop_run(); // timer,  multi task    will use this api;
    ubus_uninit();
    return 0;
}