#include <stdio.h>

/*关键头文件*/
#include <libubox/uloop.h> // uloop相关的操作
#include <libubox/blobmsg_json.h>
#include <libubus.h>

//进程中唯一，其中的fd变量需要加到epoll中
static struct ubus_context *ctx;
static struct blob_buf b;

//client端的可以实现注册函数
enum  
{  
    SCAN_CHNID,  
    SCAN_POLICY_MAX,  
};

static const struct blobmsg_policy scan_policy[] = {
    [SCAN_CHNID] = {.name = "chnID", .type = BLOBMSG_TYPE_INT32},
};

static int client_init(const char *path)
{
    uloop_init();
    ctx = ubus_connect(path);
    if (ctx == NULL) {
        exit(-1);
    }
    return 0;
}

static void scanreq_prog_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
    bool simple_output = false;
    char *str;
    if (!msg) 
        return ;
    str = blobmsg_format_json_indent(msg, true, simple_output ? -1 : 0);
    printf("%s\r\n", str);
    free(str);
}

static int client_ubus_call(void)
{
    int ret; 
    unsigned int id; 
    int timeout = 30;  
    
    blob_buf_init(&b, 0);
    blobmsg_add_u32(&b, scan_policy[SCAN_CHNID].name, 0);

    ret = ubus_lookup_id(ctx, "scan_prog", &id); 
    if (ret != UBUS_STATUS_OK) {
        printf("lookup scan_prog failed\n");  
        return ret;  
    } else {
        printf("lookup scan_prog successs\n");  
    }

    return ubus_invoke(ctx, id, "scan", b.head, scanreq_prog_cb, NULL, timeout * 1000);
}

static void client_ubus_done(void)
{
    if (ctx) 
        ubus_free(ctx);
}

int main(int argc, char **argv)
{
    client_init(argv[1]);  
    client_ubus_call();
    client_ubus_done();
    return 0;
}
