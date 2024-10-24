#include <stdio.h>


/*关键头文件*/
#include <libubox/uloop.h> // uloop相关的操作
#include <libubox/blobmsg_json.h>
#include <libubus.h>

static struct ubus_context *ctx = NULL;
static struct blob_buf b;  //必须为全局变量

static int ubus_init(const char *path)
{
    uloop_init();
    ctx = ubus_connect(path);
    if (!ctx) 
    {
        printf("ubus connect failed\r\n");
        return -1;
    }

    return 0;
}

static void ubus_uninit(void)
{
    if (ctx) ubus_free(ctx);
}

static int server_ubus_send_event(void)
{

    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, "major", 3);
    blobmsg_add_u32(&b, "minor", 56);
    blobmsg_add_string(&b, "name", "mmc01");
    return ubus_send_event(ctx, "add_device", b.head);
}

int main(int argc, char **argv)
{
    if (ubus_init(argv[1]) != UBUS_STATUS_OK) {
        return -1;
    }

    server_ubus_send_event();

    ubus_uninit();

    return 0;
}